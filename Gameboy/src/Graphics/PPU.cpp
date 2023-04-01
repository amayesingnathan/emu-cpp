module Gameboy.Graphics;

import Gameboy.Memory.AddressBus;

namespace GB {

	PPU::PPU()
		: mDisplayPB(Emu::PixelBuffer::Create(SCREEN_WIDTH, SCREEN_HEIGHT)),
		  mLCDControl(AddressBus::Read(Addr::LCDC)),
		  mLCDStatus(AddressBus::Read(Addr::STAT)),
		  mLCDScanline(AddressBus::Read(Addr::LY)),
		  mLCDComparison(AddressBus::Read(Addr::LYC))
	{
	}

	void PPU::tick(Byte cycles)
	{
		mClockCounter += cycles;
		switch (mCurrentMode)
		{
		case Mode::HBLANK:
			HBlankMode();
			break;

		case Mode::VBLANK:
			VBlankMode();
			break;

		case Mode::OAM:
			OAMMode();
			break;

		case Mode::LCD_TR:
			LCDTransferMode();
			break;
		}

		CompareScanline();
	}

	void PPU::disableLCD()
	{
		mClockCounter = 0;
		mLCDScanline = 0;
		SetLCDStatus(Mode::HBLANK);
	}

	void PPU::SetLCDStatus(Mode newMode)
	{
		ByteBits lcdStatus = mLCDStatus;
		lcdStatus &= 0xFC;

		switch (newMode)
		{
		case Mode::HBLANK:
			if (lcdStatus.test(LCD_INTERRUPT_BIT0))
				AddressBus::RequestInterrupt(Interrupt::LCD_Stat);
			break;

		case Mode::VBLANK:
			if (lcdStatus.test(LCD_INTERRUPT_BIT1))
				AddressBus::RequestInterrupt(Interrupt::LCD_Stat);
			lcdStatus.set(LCD_MODE_BIT0);
			break;

		case Mode::OAM:
			if (lcdStatus.test(LCD_INTERRUPT_BIT2))
				AddressBus::RequestInterrupt(Interrupt::LCD_Stat);
			lcdStatus.set(LCD_MODE_BIT1);
			break;

		case Mode::LCD_TR:
			lcdStatus.set(LCD_MODE_BIT0);
			lcdStatus.set(LCD_MODE_BIT1);
			break;
		}

		mCurrentMode = newMode;
		mLCDStatus = (Byte)lcdStatus.to_ulong();
	}

	void PPU::CompareScanline()
	{
		ByteBits lcdStatus = mLCDStatus;

		lcdStatus.reset(LY_COMPARISON_BIT);
		if (mLCDScanline == mLCDComparison)
		{
			lcdStatus.set(LY_COMPARISON_BIT);
			if (lcdStatus.test(LCD_INTERRUPT_BIT3))
				AddressBus::RequestInterrupt(Interrupt::LCD_Stat);
		}

		mLCDStatus = (Byte)lcdStatus.to_ulong();
	}

	void PPU::HBlankMode()
	{
		if (mClockCounter < HBLANK_CYCLES)
			return;

		DrawScanline();

		mClockCounter -= HBLANK_CYCLES;
		mLCDScanline++;

		Mode nextMode = Mode::OAM;
		if (mLCDScanline == SCANLINE_COUNT)
		{
			nextMode = Mode::VBLANK;
			AddressBus::RequestInterrupt(Interrupt::V_Blank);
		}

		SetLCDStatus(nextMode);
	}

	void PPU::VBlankMode()
	{
		if (mClockCounter < CYCLES_PER_SCANLINE)
			return;

		mClockCounter -= CYCLES_PER_SCANLINE;
		mLCDScanline++;

		if (mLCDScanline < SCANLINE_COUNT_MAX)
			return;

		DrawSprites();
		mLCDScanline = 0;

		SetLCDStatus(Mode::OAM);
	}

	void PPU::OAMMode()
	{
		if (mClockCounter < OAM_CYCLES)
			return;

		mClockCounter -= OAM_CYCLES;
		SetLCDStatus(Mode::LCD_TR);
	}

	void PPU::LCDTransferMode()
	{
		if (mClockCounter < LCD_TRANSFER_CYCLES)
			return;

		mClockCounter -= LCD_TRANSFER_CYCLES;

		SetLCDStatus(Mode::HBLANK);
	}

	void PPU::DrawScanline()
	{
		ByteBits lcdControl = mLCDControl;
		if (!lcdControl.test(LCD_ENABLE_BIT))
			return;

		DrawTiles();
	}

	void PPU::DrawTiles()
	{
		ByteBits lcdControl = mLCDControl;

		if (!lcdControl.test(BG_ENABLE_BIT))
			return;

		Word tileData = 0x8800;
		Word backgroundMemory = 0;
		bool unsig = false;

		// where to draw the visual area and the window
		Byte scrollY = AddressBus::Read(Addr::SCY);
		Byte scrollX = AddressBus::Read(Addr::SCX);
		Byte windowY = AddressBus::Read(Addr::WY);
		Byte windowX = AddressBus::Read(Addr::WX) - 7;

		bool usingWindow = false;
		if (lcdControl.test(WINDOW_ENABLE_BIT))
		{
			if (windowY <= mLCDScanline)
				usingWindow = true;
		}

		if (lcdControl.test(BG_TILEDATA_BIT))
		{
			tileData = 0x8000;
			unsig = true;
		}

		backgroundMemory = 0x9800;
		Byte testBit = (usingWindow) ? WINDOW_TILEMAP_BIT : BG_TILEMAP_BIT;
		if (lcdControl.test(testBit))
			backgroundMemory = 0x9C00;

		Byte yPos = scrollY + mLCDScanline;
		if (usingWindow)
			yPos = mLCDScanline - windowY;

		Word tileRow = (((Byte)(yPos / 8)) * 32);

		for (Byte pixel = 0; pixel < SCREEN_WIDTH; pixel++)
		{
			Byte xPos = pixel + scrollX;
			if (usingWindow && pixel >= windowX)
				xPos = pixel - windowX;

			SWord tileNum;
			Word tileLocation = tileData;
			Word tileAddress = backgroundMemory + tileRow + (xPos / 8);
			if (unsig)
			{
				tileNum = (Byte)AddressBus::Read(tileAddress);
				tileLocation += (tileNum * 16);
			}
			else
			{
				tileNum = (SByte)AddressBus::Read(tileAddress);
				tileLocation += ((tileNum + 0x80) * 16);
			}

			Byte line = (yPos % 8);
			line *= 2;
			ByteBits hiByte = AddressBus::Read(tileLocation + line);
			ByteBits loByte = AddressBus::Read(tileLocation + line + 1);

			Byte colourBit = 7 - (xPos % 8);

			Byte colourNum = loByte.test(colourBit) ? 1 : 0;
			colourNum <<= 1;
			colourNum |= hiByte.test(colourBit) ? 1 : 0;

			Colour col = GetColour(colourNum, Addr::BGP);
			Emu::Pixel colour = WHITE;

			// setup the RGB values
			switch (col)
			{
			case Colour::WHITE:		 colour = WHITE;	  break;
			case Colour::LIGHT_GREY: colour = LIGHT_GREY; break;
			case Colour::DARK_GREY:  colour = DARK_GREY;  break;
			case Colour::BLACK:		 colour = BLACK;	  break;
			}

			if ((mLCDScanline < 0) || (mLCDScanline > 143) || (pixel < 0) || (pixel > 159))
				continue;

			mDisplayPB->at(pixel, mLCDScanline) = colour;
		}
	}

	void PPU::DrawSprites()
	{
		ByteBits lcdControl = mLCDControl;

		if (lcdControl.test(OBJ_ENABLE_BIT))
			return;

		bool use8x16 = false;
		if (lcdControl.test(OBJ_SIZE_BIT))
			use8x16 = true;

		for (Byte sprite = 0; sprite < 40; sprite++)
		{
			Byte index = sprite * 4;
			Sprite spriteData = AddressBus::ReadSprite(0xFE00 + index);
			spriteData.yPos -= 16;
			spriteData.xPos -= 8;

			bool yFlip = spriteData.flags.test(Y_FLIP_BIT);
			bool xFlip = spriteData.flags.test(X_FLIP_BIT);

			Byte ysize = 8;
			if (use8x16)
				ysize = 16;

			if ((mLCDScanline >= spriteData.yPos) && (mLCDScanline < (spriteData.yPos + ysize)))
			{
				Byte line = mLCDScanline - spriteData.yPos;
				if (yFlip)
					line = ysize - line;

				line *= 2;
				Word dataAddress = (0x8000 + (spriteData.tileLoc * 16)) + line;
				ByteBits data1 = AddressBus::Read(dataAddress);
				ByteBits data2 = AddressBus::Read(dataAddress + 1);

				for (Byte tilePixel = 0; tilePixel < 8; tilePixel++)
				{
					Byte colourBit = tilePixel;
					if (!xFlip)
						colourBit = 7 - colourBit;

					Byte colourNum = data2.test(colourBit) ? 1 : 0;
					colourNum <<= 1;
					colourNum |= data1.test(colourBit) ? 1 : 0;

					Word colourAddress = spriteData.flags.test(PALETTE_NUM_BIT) ? Addr::OBP1 : Addr::OBP0;
					Colour col = GetColour(colourNum, colourAddress);

					// white is transparent for sprites.
					if (col == Colour::WHITE)
						continue;

					Emu::Pixel colour = WHITE;
					switch (col)
					{
					case Colour::WHITE:		 colour = WHITE;	  break;
					case Colour::LIGHT_GREY: colour = LIGHT_GREY; break;
					case Colour::DARK_GREY:  colour = DARK_GREY;  break;
					case Colour::BLACK:		 colour = BLACK;	  break;
					}

					Byte pixel = spriteData.xPos + tilePixel;

					if ((mLCDScanline < 0) || (mLCDScanline > 143) || (pixel < 0) || (pixel > 159))
						continue;

					mDisplayPB->at(pixel, mLCDScanline) = colour;
				}
			}
		}
	}

	PPU::Colour PPU::GetColour(Byte colourNum, Word address)
	{
		Colour res = Colour::WHITE;
		ByteBits palette = AddressBus::Read(address);
		int hi = 0;
		int lo = 0;

		switch (colourNum)
		{
		case 0: hi = 1; lo = 0; break;
		case 1: hi = 3; lo = 2; break;
		case 2: hi = 5; lo = 4; break;
		case 3: hi = 7; lo = 6; break;
		}

		// use the palette to get the colour
		Byte colour = palette.test(hi) ? 1 : 0;
		colour <<= 1;
		colour |= palette.test(lo) ? 1 : 0;

		// convert the game colour to emulator colour
		switch (colour)
		{
		case 0: res = Colour::WHITE; break;
		case 1: res = Colour::LIGHT_GREY; break;
		case 2: res = Colour::DARK_GREY; break;
		case 3: res = Colour::BLACK; break;
		}

		return res;
	}
}
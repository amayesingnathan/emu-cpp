#include "gbpch.h"
#include "PPU.h"

#include "Graphics/Flags.h"
#include "Memory/Address.h"
#include "Memory/AddressBus.h"

namespace GB {

	PPU::PPU()
		: mDisplayPB(Emu::PixelBuffer::Create(SCREEN_WIDTH, SCREEN_HEIGHT))
	{
	}

	PPU::~PPU()
	{
	}

	void PPU::update(Byte cycles)
	{
		if (!IsLCDEnabled())
		{
			DisableLCD();
			return;
		}

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
	}

	bool PPU::IsLCDEnabled()
	{
		BitField lcdControl = AddressBus::Read(Addr::LCDC);
		return lcdControl.bit(LCD_ENABLE_BIT);
	}

	void PPU::DisableLCD()
	{
		mClockCounter = 0;
		AddressBus::Write(Addr::LY, 0);
		SetLCDStatus(Mode::HBLANK);
	}

	void PPU::SetLCDStatus(Mode newMode)
	{
		BitField lcdStatus = AddressBus::Read(Addr::LCDS);
		lcdStatus &= 0xFC;

		switch (newMode)
		{
		case Mode::HBLANK:
			if (lcdStatus.bit(LCD_INTERRUPT_BIT0))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
			break;

		case Mode::VBLANK:
			if (lcdStatus.bit(LCD_INTERRUPT_BIT1))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
			lcdStatus.set(LCD_MODE_BIT0);
			break;

		case Mode::OAM:
			if (lcdStatus.bit(LCD_INTERRUPT_BIT2))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
			lcdStatus.set(LCD_MODE_BIT1);
			break;

		case Mode::LCD_TR:
			lcdStatus.set(LCD_MODE_BIT0);
			lcdStatus.set(LCD_MODE_BIT1);
			break;
		}

		mCurrentMode = newMode;
		AddressBus::Write(Addr::LCDS, lcdStatus);
	}

	void PPU::CompareScanline(Byte scanline)
	{
		Byte scanlineComparison = AddressBus::Read(Addr::LYC);
		Byte& lcds = AddressBus::Read(Addr::LCDS);
		BitField lcdStatus = lcds;

		lcdStatus.reset(LY_COMPARISON_BIT);
		if (scanline == scanlineComparison)
		{
			lcdStatus.set(LY_COMPARISON_BIT);
			if (lcdStatus.bit(LCD_INTERRUPT_BIT3))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
		}

		lcds = lcdStatus;
	}

	void PPU::HBlankMode()
	{
		if (mClockCounter < HBLANK_CYCLES)
			return;

		mClockCounter -= HBLANK_CYCLES;
		Byte& scanline = ++AddressBus::Read(Addr::LY);
		CompareScanline(scanline);

		Mode nextMode = Mode::OAM;
		if (scanline == SCANLINE_COUNT)
		{
			nextMode = Mode::VBLANK;
			AddressBus::RequestInterrupt(Interrupt::VBLANK);
		}

		SetLCDStatus(nextMode);

	}

	void PPU::VBlankMode()
	{
		if (mClockCounter < CYCLES_PER_SCANLINE)
			return;

		mClockCounter -= CYCLES_PER_SCANLINE;
		Byte& scanline = ++AddressBus::Read(Addr::LY);
		CompareScanline(scanline);

		if (scanline < SCANLINE_COUNT_MAX)
			return;

 		mClockCounter = 0;
		scanline = 0;
		CompareScanline(scanline);

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
		DrawScanline();
	}

	void PPU::DrawScanline()
	{
		BitField lcdControl = AddressBus::Read(Addr::LCDC);

		if (!lcdControl.bit(LCD_ENABLE_BIT))
			return;

		DrawTiles(lcdControl);
		DrawSprites(lcdControl);
	}

	void PPU::DrawTiles(BitField lcdControl)
	{
		if (!lcdControl.bit(BG_ENABLE_BIT))
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
		if (lcdControl.bit(WINDOW_ENABLE_BIT))
		{
			if (windowY <= AddressBus::Read(Addr::LY))
				usingWindow = true;
		}

		if (lcdControl.bit(BG_TILEDATA_BIT))
		{
			tileData = 0x8000;
			unsig = true;
		}

		backgroundMemory = 0x9800;
		Byte testBit = (usingWindow) ? WINDOW_TILEMAP_BIT : BG_TILEMAP_BIT;
		if (lcdControl.bit(testBit))
			backgroundMemory = 0x9C00;

		Byte scanline = AddressBus::Read(Addr::LY);

		Byte yPos = scrollY + scanline;
		if (usingWindow)
			yPos = scanline - windowY;

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
			BitField hiByte = AddressBus::Read(tileLocation + line);
			BitField loByte = AddressBus::Read(tileLocation + line + 1);

			Byte colourBit = 7 - (xPos % 8);

			Byte colourNum = loByte.val(colourBit);
			colourNum <<= 1;
			colourNum |= hiByte.val(colourBit);

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

			if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
				continue;

			mDisplayPB->at(pixel, scanline) = colour;
		}
	}

	void PPU::DrawSprites(BitField lcdControl)
	{
		if (lcdControl.bit(OBJ_ENABLE_BIT))
			return;

		bool use8x16 = false;
		if (lcdControl.bit(OBJ_SIZE_BIT))
			use8x16 = true;

		for (Byte sprite = 0; sprite < 40; sprite++)
		{
			Byte index = sprite * 4;
			Sprite spriteData = AddressBus::ReadSprite(0xFE00 + index); 
			spriteData.yPos -= 16;
			spriteData.xPos -= 8;

			bool yFlip = spriteData.flags.bit(Y_FLIP_BIT);
			bool xFlip = spriteData.flags.bit(X_FLIP_BIT);

			Byte scanline = AddressBus::Read(Addr::LY);

			Byte ysize = 8;
			if (use8x16)
				ysize = 16;

			if ((scanline >= spriteData.yPos) && (scanline < (spriteData.yPos + ysize)))
			{
				Byte line = scanline - spriteData.yPos;
				if (yFlip)
					line = ysize - line;

				line *= 2;
				Word dataAddress = (0x8000 + (spriteData.tileLoc * 16)) + line;
				BitField data1 = AddressBus::Read(dataAddress);
				BitField data2 = AddressBus::Read(dataAddress + 1);

				for (Byte tilePixel = 0; tilePixel < 8; tilePixel++)
				{
					Byte colourBit = tilePixel;
					if (!xFlip)
						colourBit = 7 - colourBit;

					Byte colourNum = data2.val(colourBit);
					colourNum <<= 1;
					colourNum |= data1.val(colourBit);

					Word colourAddress = spriteData.flags.bit(PALETTE_NUM_BIT) ? Addr::OBP1 : Addr::OBP0;
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

					if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
						continue;

					mDisplayPB->at(pixel, scanline) = colour;
				}
			}
		}
	}

	PPU::Colour PPU::GetColour(Byte colourNum, Word address)
	{
		Colour res = Colour::WHITE;
		BitField palette = AddressBus::Read(address);
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
		Byte colour = 0;
		colour = palette.val(hi) << 1;
		colour |= palette.val(lo);

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
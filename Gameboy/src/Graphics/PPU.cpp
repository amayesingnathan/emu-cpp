#include "gbpch.h"
#include "PPU.h"

#include "Graphics/Flags.h"
#include "Memory/Address.h"
#include "Memory/AddressBus.h"

namespace GB {

	PPU::PPU()
		: mDisplayPB(Emu::PixelBuffer::Create(_ScreenWidth, _ScreenHeight))
	{
	}

	PPU::~PPU()
	{
	}

	void PPU::update(Byte cycles)
	{
		Byte& scanline = AddressBus::Read(Addr::LY);
		BitField lcdStatus = AddressBus::Read(Addr::LCDS);
		if (!IsLCDEnabled())
		{
			DisableLCD();
			return;
		}

		mClockCounter += cycles;
		switch (mCurrentMode)
		{
		case HBLANK:
			HBlankMode();
			break;

		case VBLANK:
			VBlankMode();
			break;

		case OAM:
			OAMMode();
			break;

		case LCD_TR:
			LCDTransferMode();
			break;
		}
	}

	bool PPU::IsLCDEnabled()
	{
		BitField lcdControl = AddressBus::Read(Addr::LCDC);
		return lcdControl.bit(_LCDEnableBit);
	}

	void PPU::DisableLCD()
	{
		mClockCounter = 0;
		AddressBus::Write(Addr::LY, 0);
		SetLCDStatus(HBLANK);
	}

	void PPU::SetLCDStatus(Mode newMode)
	{
		BitField lcdStatus = AddressBus::Read(Addr::LCDS);
		lcdStatus &= 0xFC;

		switch (newMode)
		{
		case HBLANK:
			if (lcdStatus.bit(3))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
			break;

		case VBLANK:
			if (lcdStatus.bit(4))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
			lcdStatus.set(0);
			break;

		case OAM:
			if (lcdStatus.bit(5))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
			lcdStatus.set(1);
			break;

		case LCD_TR:
			lcdStatus.set(0);
			lcdStatus.set(1);
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

		lcdStatus.reset(2);
		if (scanline == scanlineComparison)
		{
			lcdStatus.set(2);
			if (lcdStatus.bit(6))
				AddressBus::RequestInterrupt(Interrupt::LCD_STAT);
		}

		lcds = lcdStatus;
	}

	void PPU::HBlankMode()
	{
		if (mClockCounter < _HBlankCycles)
			return;

		mClockCounter -= _HBlankCycles;
		Byte& scanline = ++AddressBus::Read(Addr::LY);
		CompareScanline(scanline);

		Mode nextMode = OAM;
		if (scanline == _ScanlineCount)
		{
			nextMode = HBLANK;
			AddressBus::RequestInterrupt(Interrupt::VBLANK);
		}

		SetLCDStatus(nextMode);

	}

	void PPU::VBlankMode()
	{
		if (mClockCounter < _CyclesPerScanline)
			return;

		mClockCounter -= _CyclesPerScanline;
		Byte& scanline = ++AddressBus::Read(Addr::LY);
		CompareScanline(scanline);

		if (scanline < _ScanlineCountMax)
			return;

 		mClockCounter = 0;
		scanline = 0;
		CompareScanline(scanline);

		SetLCDStatus(OAM);

	}

	void PPU::OAMMode()
	{
		if (mClockCounter < _OAMCycles)
			return;

		mClockCounter -= _OAMCycles;
		SetLCDStatus(LCD_TR);
	}

	void PPU::LCDTransferMode()
	{
		if (mClockCounter < _LCDTransferCycles)
			return;

		mClockCounter -= _LCDTransferCycles;
		SetLCDStatus(HBLANK);
		DrawScanline();
	}

	void PPU::DrawScanline()
	{
		BitField lcdControl = AddressBus::Read(Addr::LCDC);

		if (lcdControl.bit(_BGEnableBit))
			DrawTiles(lcdControl);
		if (lcdControl.bit(_OBJEnableBit))
			DrawSprites(lcdControl);
	}

	void PPU::DrawTiles(BitField lcdControl)
	{
		Word tileData = 0x8000;
		Word backgroundMemory = 0;
		bool unsig = true;

		// where to draw the visual area and the window
		Byte scrollY = AddressBus::Read(Addr::SCY);
		Byte scrollX = AddressBus::Read(Addr::SCX);
		Byte windowY = AddressBus::Read(Addr::WY);
		Byte windowX = AddressBus::Read(Addr::WX) - 7;

		bool usingWindow = false;
		if (lcdControl.bit(_WindowEnableBit))
		{
			if (windowY <= AddressBus::Read(Addr::LY))
				usingWindow = true;
		}

		if (!lcdControl.bit(_BGTileDataBit))
		{
			tileData = 0x8800;
			unsig = false;
		}

		backgroundMemory = 0x9800;
		Byte testBit = (usingWindow) ? _WindowTilemapBit : _BGTilemapBit;
		if (lcdControl.bit(testBit))
			backgroundMemory = 0x9C00;

		Byte scanline = AddressBus::Read(Addr::LY);

		Byte yPos = scrollY + scanline;
		if (usingWindow)
			yPos = scanline - windowY;

		Word tileRow = yPos * 4;

		for (Byte pixel = 0; pixel < _ScreenWidth; pixel++)
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

			Byte line = (yPos % 8) * 2;
			BitField hiByte = AddressBus::Read(tileLocation + line);
			BitField loByte = AddressBus::Read(tileLocation + line + 1);

			Byte colourBit = 7 - (xPos % 8);

			Byte colourNum = loByte.val(colourBit);
			colourNum <<= 1;
			colourNum |= hiByte.val(colourBit);

			Colour col = GetColour(colourNum, Addr::BGP);
			Emu::Pixel colour = _White;

			// setup the RGB values
			switch (col)
			{
			case WHITE:		 colour = _White;	  break;
			case LIGHT_GREY: colour = _LightGrey; break;
			case DARK_GREY:  colour = _DarkGrey;  break;
			case BLACK:		 colour = _Black;	  break;
			}

			int finalY = AddressBus::Read(Addr::LY);
			if ((finalY < 0) || (finalY > 143) || (pixel < 0) || (pixel > 159))
				continue;

			mDisplayPB->at(pixel, finalY) = colour;
		}
	}

	void PPU::DrawSprites(BitField lcdControl)
	{
		bool use8x16 = false;
		if (lcdControl.bit(_OBJSizeBit))
			use8x16 = true;

		for (Byte sprite = 0; sprite < 40; sprite++)
		{
			Byte index = sprite * 4;
			Sprite spriteData = AddressBus::ReadSprite(0xFE00 + index); 
			spriteData.yPos -= 16;
			spriteData.xPos -= 8;

			bool yFlip = spriteData.flags.bit(_YFlipBit);
			bool xFlip = spriteData.flags.bit(_XFlipBit);

			int scanline = AddressBus::Read(Addr::LY);

			int ysize = 8;
			if (use8x16)
				ysize = 16;

			if ((scanline >= spriteData.yPos) && (scanline < (spriteData.yPos + ysize)))
			{
				int line = scanline - spriteData.yPos;
				if (yFlip)
				{
					line -= ysize;
					line *= -1;
				}

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

					Word colourAddress = spriteData.flags.bit(_PaletteNum) ? Addr::OBP1 : Addr::OBP0;
					Colour col = GetColour(colourNum, colourAddress);

					// white is transparent for sprites.
					if (col == WHITE)
						continue;

					Emu::Pixel colour = _White;
					switch (col)
					{
					case WHITE:		 colour = _White;	  break;
					case LIGHT_GREY: colour = _LightGrey; break;
					case DARK_GREY:  colour = _DarkGrey;  break;
					case BLACK:		 colour = _Black;	  break;
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
		Colour res = WHITE;
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
		int colour = 0;
		colour = palette.val(hi) << 1;
		colour |= palette.val(lo);

		// convert the game colour to emulator colour
		switch (colour)
		{
		case 0: res = WHITE; break;
		case 1: res = LIGHT_GREY; break;
		case 2: res = DARK_GREY; break;
		case 3: res = BLACK; break;
		}

		return res;
	}
}
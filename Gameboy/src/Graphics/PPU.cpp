#include "gbpch.h"
#include "PPU.h"

#include "Graphics/Flags.h"
#include "Memory/AddressBus.h"

namespace GB {

	PPU::PPU()
		: mPixelBuffer(Emu::PixelBuffer::Create(_BufSize)), mDisplayTexture(Emu::Texture::Create(Screen::_Width, Screen::_Height))
	{
	}

	PPU::~PPU()
	{
	}

	void PPU::update(Byte cycles)
	{
		BitField lcdStatus = AddressBus::Read(Addr::LCDS);
		if (!IsLCDEnabled())
		{
			mClockCounter = 0;
			AddressBus::Write(Addr::SCANL, 0);
			lcdStatus &= 0xFC;
			lcdStatus.set(0);
			AddressBus::Write(Addr::LCDS, lcdStatus);
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

	void PPU::HBlankMode()
	{
		if (mClockCounter < _HBlankCycles)
			return;

		mClockCounter -= _HBlankCycles;
		Byte& scanline = AddressBus::Read(Addr::SCANL);
		scanline++;


	}

	void PPU::VBlankMode()
	{
		if (mClockCounter < _CyclesPerScanline)
			return;

		mClockCounter -= _CyclesPerScanline;
		Byte& scanline = ++AddressBus::Read(Addr::SCANL);

		if (scanline < _ScanlineCountMax)
			return;

		DrawSprites();

		BitField lcdStatus = AddressBus::Read(Addr::LCDS);
		mClockCounter = 0;
		AddressBus::Write(Addr::SCANL, 0);
		lcdStatus &= 0xFC;
		lcdStatus.set(1);
		AddressBus::Write(Addr::LCDS, lcdStatus);

	}

	void PPU::OAMMode()
	{
	}

	void PPU::LCDTransferMode()
	{
	}
	void PPU::DrawSprites()
	{
	}
}
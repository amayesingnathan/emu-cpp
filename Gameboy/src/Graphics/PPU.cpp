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
		SetLCDStatus();

		if (!IsLCDEnabled())
			return;

		mClockCounter += cycles;
		
		if (mClockCounter < _CyclesPerScanline)
			return;



	}

	void PPU::SetLCDStatus()
	{
	}

	bool PPU::IsLCDEnabled()
	{
		BitField lcdControl = AddressBus::Read(Addr::LCDC);
		return lcdControl.bit(_LCDEnableBit);
	}
}
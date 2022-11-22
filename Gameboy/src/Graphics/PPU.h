#pragma once

#include "Graphics/PixelBuffer.h"

#include "Screen.h"

namespace GB {

	class PPU
	{
	public:
		GB_CONST USize _BufSize = Screen::_Width * Screen::_Height * sizeof(Emu::Pixel);

		GB_CONST Word _CyclesPerScanline = 456;

		enum Mode : Byte
		{
			HBLANK = 0, VBLANK, OAM, LCD_TR
		};

	public:
		PPU();
		~PPU();

	public:
		void update(Byte cycles);

	private:
		void BeginFrame() { mDisplay = mPixelBuffer->lock(); }
		void EndFrame() { mPixelBuffer->unlock(); }

		void SetLCDStatus();
		bool IsLCDEnabled();

	private:
		Ref<Emu::PixelBuffer> mPixelBuffer;
		Screen mDisplay;

		Word mClockCounter = 0;
	};
}
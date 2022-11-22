#pragma once

#include "Graphics/PixelBuffer.h"

#include "Screen.h"

namespace GB {

	class PPU
	{
	public:
		GB_CONST USize _BufSize = Screen::_Width * Screen::_Height * sizeof(Emu::Pixel);

	public:
		PPU();
		~PPU();

	public:
		void update(int cycles);

	private:
		Ref<Emu::PixelBuffer> mPixelBuffer;
		Screen mDisplay;
	};
}
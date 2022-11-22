#include "gbpch.h"
#include "PPU.h"

#include "Graphics/Renderer.h"

namespace GB {

	PPU::PPU()
		: mPixelBuffer(Emu::PixelBuffer::Create(_BufSize))
	{
	}

	PPU::~PPU()
	{
	}

	void PPU::update(int cycles)
	{
		mDisplay = mPixelBuffer->lock();

		for (USize y = 0; y < Screen::_Height; y++)
		{
			for (USize x = 0; x < Screen::_Width; x++)
			{
				Emu::Pixel& pixel = mDisplay(x, y);
				pixel.r = ((x * y) * (x + y)) % 255;
				pixel.g = ((x * y) - (x + y)) % 255;
				pixel.b = ((x * y) + (x * y)) % 255;
				pixel.a = 255;
			}
		}

		mPixelBuffer->unlock();
	}
}
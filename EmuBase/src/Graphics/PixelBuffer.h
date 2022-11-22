#pragma once

#include "Core/Common.h"

namespace Emu {

	class Texture;

	struct Pixel
	{
		uint8_t b, g, r, a;
	};

	class PixelBuffer
	{
	public:
		PixelBuffer(uint size);
		~PixelBuffer();

		uint8_t* lock();
		void unlock();

		void upload(Ref<Texture> tex);

		static Ref<PixelBuffer> Create(uint size) { return std::make_shared<PixelBuffer>(size); }

	private:
		uint mRendererID = 0;
		uint mSize = 0;

		bool mLocked = false;
	};
}
#pragma once

#include "Core/Common.h"

namespace Emu {

	class Texture;

	struct Pixel
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

#define _Map(x, y) ((y * mWidth) + x)

	class PixelBuffer
	{
	public:
		PixelBuffer(uint width, uint height);
		~PixelBuffer();

		void lock();
		void unlock();

		uint getTexID() const;

		Pixel& at(usize x, usize y);
		const Pixel& at(usize x, usize y) const;

		static Ref<PixelBuffer> Create(uint width, uint height) { return std::make_shared<PixelBuffer>(width, height); }

	private:
		uint mRendererID = 0;
		uint mSize = 0;
		uint mWidth = 0, mHeight = 0;

		bool mLocked = false;

		Ref<Texture> mTexture = nullptr;
		Pixel* mPixels = nullptr;
	};
}
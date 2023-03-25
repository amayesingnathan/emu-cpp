export module EmuBase.Graphics.PixelBuffer;

import EmuBase.Core.Common;
import EmuBase.Graphics.Texture;
export import <cstdint>;

export namespace Emu {

	struct Pixel
	{
		uint8_t r, g, b, a;
	};

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

		Pixel* get(usize offset = 0) { return mPixels + offset; }
		const Pixel* get(usize offset = 0) const { return mPixels + offset; }
		void set(const Pixel& colour);
		void set(const Pixel& colour, usize size, usize offset);

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
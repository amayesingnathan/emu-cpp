#pragma once

#include "Core/Common.h"

namespace Emu {

	class Texture
	{
	public:
		Texture(uint width, uint height);
		~Texture();

		void bind();

		uint getWidth() const { return mWidth; }
		uint getHeight() const { return mHeight; }

		uint getTexID() const { return mRendererID; }

		static Ref<Texture> Create(uint width, uint height) { return std::make_shared<Texture>(width, height); }

	private:
		uint mRendererID = -1;
		uint mWidth, mHeight;
	};
}
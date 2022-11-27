#include "glpch.h"
#include "Texture.h"

#include "glad/glad.h"

namespace Emu {

	Texture::Texture(uint width, uint height)
		: mWidth(width), mHeight(height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, GL_RGBA8, mWidth, mHeight);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		uint8_t* data = new uint8_t[width * height * 4];
		memset(data, 0xFF, width * height * 4);
		glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
		delete[] data;
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &mRendererID);
	}

	void Texture::bind()
	{
		glBindTexture(GL_TEXTURE_2D, mRendererID);
	}
}
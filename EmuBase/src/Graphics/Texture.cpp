#include "glpch.h"
#include "Texture.h"

#include "glad/glad.h"

namespace Emu {

	Texture::Texture(uint width, uint height)
		: mWidth(width), mHeight(height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, GL_BGRA, width, height);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &mRendererID);
	}

	void Texture::bind(uint slot)
	{
		glBindTextureUnit(slot, mRendererID);
	}
}
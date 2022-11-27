#include "glpch.h"
#include "PixelBuffer.h"

#include "Texture.h"

#include "glad/glad.h"

namespace Emu {

	PixelBuffer::PixelBuffer(uint width, uint height)
        : mWidth(width), mHeight(height), mSize(width * height * sizeof(Pixel)), mTexture(Texture::Create(width, height))
    {
        glGenBuffers(1, &mRendererID);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, mSize, nullptr, GL_STREAM_DRAW);
    }

    PixelBuffer::~PixelBuffer()
    {
        glDeleteBuffers(1, &mRendererID);
    }

    void PixelBuffer::lock()
    {
        GL_ASSERT(!mLocked, "Buffer memory already mapped!");

        mLocked = true;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, mSize, nullptr, GL_STREAM_DRAW);
        void* data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        GL_ASSERT(data, "Could not read data from pixel buffer!");
        mPixels = (Pixel*)data;
    }

    void PixelBuffer::unlock()
    {
        GL_ASSERT(mLocked, "Buffer memory not mapped!");

        mLocked = false;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        int success = glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        GL_ASSERT(success, "Could not unmap buffer");

        glTextureSubImage2D(mTexture->getTexID(), 0, 0, 0, mWidth, mHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    uint PixelBuffer::getTexID() const { return mTexture->getTexID(); }

    Pixel& PixelBuffer::at(usize x, usize y)
    {
        GL_ASSERT(mLocked, "Buffer must be locked!");
        return mPixels[_Map(x, y)];
    }

    const Pixel& PixelBuffer::at(usize x, usize y) const
    {
        GL_ASSERT(mLocked, "Buffer must be locked!");
        return mPixels[_Map(x, y)];
    }
}
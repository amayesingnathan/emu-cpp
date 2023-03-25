module;

#include "glad/glad.h"

module EmuBase.Graphics.PixelBuffer;

namespace Emu {

#define MAP(x, y) ((y * mWidth) + x)

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
        Assert(!mLocked, "Buffer memory already mapped!");

        mLocked = true;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, mSize, nullptr, GL_STREAM_DRAW);
        void* data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        Assert(data, "Could not read data from pixel buffer!");
        mPixels = (Pixel*)data;
    }

    void PixelBuffer::unlock()
    {
        Assert(mLocked, "Buffer memory not mapped!");

        mLocked = false;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        int success = glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        Assert(success, "Could not unmap buffer");

        glTextureSubImage2D(mTexture->getTexID(), 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    uint PixelBuffer::getTexID() const { return mTexture->getTexID(); }

    Pixel& PixelBuffer::at(usize x, usize y)
    {
        Assert(mLocked, "Buffer must be locked!");
        return mPixels[MAP(x, y)];
    }

    const Pixel& PixelBuffer::at(usize x, usize y) const
    {
        Assert(mLocked, "Buffer must be locked!");
        return mPixels[MAP(x, y)];
    }

    void PixelBuffer::set(const Pixel& colour)
    {
        Assert(mLocked, "Buffer must be locked!");
        Pixel* buffer = new Pixel[mWidth * mHeight](colour);
        memcpy(mPixels, buffer, mSize);
        delete[] buffer;
    }

    void PixelBuffer::set(const Pixel& colour, usize size, usize offset)
    {
        Assert(mLocked, "Buffer must be locked!");
        Pixel* buffer = new Pixel[size](colour);
        memcpy(mPixels + offset, buffer, size * sizeof(Pixel));
        delete[] buffer;
    }
}
#include "glpch.h"
#include "PixelBuffer.h"

#include "Texture.h"

#include "glad/glad.h"

namespace Emu {

	PixelBuffer::PixelBuffer(uint size)
        : mSize(size)
    {
        glGenBuffers(1, &mRendererID);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, mSize, nullptr, GL_STREAM_DRAW);
    }

    PixelBuffer::~PixelBuffer()
    {
        glDeleteBuffers(1, &mRendererID);
    }

    uint8_t* PixelBuffer::lock()
    {
        GL_ASSERT(!mLocked, "Buffer memory already mapped!");

        mLocked = true;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, mSize, nullptr, GL_STREAM_DRAW);
        void* data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        GL_ASSERT(data, "Could not read data from pixel buffer!");
        return (uint8_t*)data;
    }

    void PixelBuffer::unlock()
    {
        GL_ASSERT(mLocked, "Buffer memory not mapped!");

        mLocked = false;

        int success = glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    void PixelBuffer::upload(Ref<Texture> tex)
    {
        if (mLocked)
            unlock();

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mRendererID);
        tex->bind(0);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->getWidth(), tex->getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
}
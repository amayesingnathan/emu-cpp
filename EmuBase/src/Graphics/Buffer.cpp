#include "glpch.h"
#include "Buffer.h"

namespace GL {

    void VertexBuffer::bind() const
    {
    }

    void VertexBuffer::unbind() const
    {
    }

    void VertexBuffer::setData(const void* data, uint size)
    {
    }

    const BufferLayout& VertexBuffer::getLayout() const
    {
        // TODO: insert return statement here
    }

    void VertexBuffer::setLayout(const BufferLayout& layout)
    {
    }

    void IndexBuffer::bind() const
    {
    }

    void IndexBuffer::unbind() const
    {
    }

    uint IndexBuffer::getCount() const
    {
        return uint();
    }
}
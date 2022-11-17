#include "glpch.h"
#include "VertexArray.h"

namespace GL {

	void VertexArray::bind() const
	{
	}

	void VertexArray::unbind() const
	{
	}

	void VertexArray::addVertexBuffer(VertexBuffer* vertexBuffer)
	{
	}

	void VertexArray::setIndexBuffer(IndexBuffer* indexBuffer)
	{
	}

	const std::vector<VertexBuffer*>& VertexArray::getVertexBuffers() const
	{
		// TODO: insert return statement here
	}

	IndexBuffer* VertexArray::getIndexBuffer() const
	{
		return nullptr;
	}
}
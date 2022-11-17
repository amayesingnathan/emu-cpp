#include "glpch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace GL {

	static GLenum ShaderDataTypetoOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
		}

		GL_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &mRendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &mRendererID);
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(mRendererID);
	}

	void VertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::addVertexBuffer(VertexBuffer* vertexBuffer)
	{
		GL_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout!");

		glBindVertexArray(mRendererID);
		vertexBuffer->bind();

		const auto& layout = vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			switch (element.type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribPointer(mVertexBufferIndex,
					element.getComponentCount(),
					ShaderDataTypetoOpenGLType(element.type),
					element.normalised ? GL_TRUE : GL_FALSE,
					layout.getStride(),
					(const void*)element.offset);
				mVertexBufferIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribIPointer(mVertexBufferIndex,
					element.getComponentCount(),
					ShaderDataTypetoOpenGLType(element.type),
					layout.getStride(),
					(const void*)element.offset);
				mVertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.getComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(mVertexBufferIndex);
					glVertexAttribPointer(mVertexBufferIndex,
						count,
						ShaderDataTypetoOpenGLType(element.type),
						element.normalised ? GL_TRUE : GL_FALSE,
						layout.getStride(),
						(const void*)(sizeof(float) * count * i));
					glVertexAttribDivisor(mVertexBufferIndex, 1);
					mVertexBufferIndex++;
				}
				break;
			}
			default:
				GL_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		mVertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::setIndexBuffer(IndexBuffer* indexBuffer)
	{
		glBindVertexArray(mRendererID);
		indexBuffer->bind();

		mIndexBuffer = indexBuffer;
	}
}
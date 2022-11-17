#pragma once

#include "Buffer.h"

namespace GL {

	class VertexArray
	{
	public:
		~VertexArray() = default;

		void bind() const;
		void unbind() const;

		void addVertexBuffer(VertexBuffer* vertexBuffer);
		void setIndexBuffer(IndexBuffer* indexBuffer);

		const std::vector<VertexBuffer*>& getVertexBuffers() const;
		IndexBuffer* getIndexBuffer() const;

	private:
		uint mRendererID;
		uint mVertexBufferIndex = 0;
		std::vector<VertexBuffer*> mVertexBuffers;
		IndexBuffer* mIndexBuffer;
	};
}
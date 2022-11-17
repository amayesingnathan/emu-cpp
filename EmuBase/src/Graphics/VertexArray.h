#pragma once

#include "Buffer.h"

namespace GL {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void bind() const;
		void unbind() const;

		void addVertexBuffer(Ref<VertexBuffer> vertexBuffer);
		void setIndexBuffer(Ref<IndexBuffer> indexBuffer);

		const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return mVertexBuffers; }
		const Ref<IndexBuffer>& getIndexBuffer() const { return mIndexBuffer; }

		static Ref<VertexArray> Create() { return std::make_shared<VertexArray>(); }

	private:
		uint mRendererID;
		uint mVertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> mVertexBuffers;
		Ref<IndexBuffer> mIndexBuffer;
	};
}
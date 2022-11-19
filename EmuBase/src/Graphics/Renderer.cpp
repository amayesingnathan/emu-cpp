#include "glpch.h"
#include "Renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexArray.h"
#include "Shader.h"

namespace Emu {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		std::cout << message; // TODO Logging
		GL_ASSERT(false, "Unknown severity level!");
	}

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 colour;
	};

	struct RenderData
	{
		static const uint MaxQuads = 20000;
		static const uint MaxVertices = MaxQuads * 4;
		static const uint MaxIndices = MaxQuads * 6;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;

		uint quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		glm::vec4 quadVertexPositions[4];
	};

	static RenderData sData;

	void Renderer::Init()
	{
#ifdef LAB_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_LINE_SMOOTH);

		sData.quadVertexArray = VertexArray::Create();

		sData.quadVertexBuffer = VertexBuffer::Create(sData.MaxVertices * sizeof(QuadVertex));
		sData.quadVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "aPosition"	  },
			{ ShaderDataType::Float4, "aColour"		  },
			{ ShaderDataType::Float2, "aTexCoord"	  },
			{ ShaderDataType::Float,  "aTexIndex"	  },
			{ ShaderDataType::Float,  "aTilingFactor" },
			{ ShaderDataType::Int,    "aEntityID"	  }
			});

		sData.quadVertexArray->addVertexBuffer(sData.quadVertexBuffer);

		sData.quadVertexBufferBase = new QuadVertex[sData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[sData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < sData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, sData.MaxIndices);
		sData.quadVertexArray->setIndexBuffer(quadIB);

		delete[] quadIndices;

		sData.quadShader = Shader::Create("shaders/quad.glsl");

		sData.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer::Shutdown()
	{
		delete[] sData.quadVertexBufferBase;
	}

	void Renderer::BeginFrame()
	{
		sData.quadShader->bind();
		sData.quadShader->setMat4("uViewProjection", glm::mat4(1.0f));

		StartBatch();
	}

	void Renderer::EndFrame()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)sData.quadVertexBufferPtr - (uint8_t*)sData.quadVertexBufferBase);
		sData.quadVertexBuffer->setData(sData.quadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer::DrawPixel(usize x, usize y, const glm::vec4& colour)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { (float)x, (float)y, 0 });
		DrawPixelImpl(transform, colour);
	}

	void Renderer::DrawPixelImpl(const glm::mat4& transform, const glm::vec4& colour)
	{
		constexpr usize quadVertexCount = 4;

		if (sData.quadIndexCount >= RenderData::MaxIndices)
			NextBatch();

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sData.quadVertexBufferPtr->position = transform * sData.quadVertexPositions[i];
			sData.quadVertexBufferPtr->colour = colour;
			sData.quadVertexBufferPtr++;
		}

		sData.quadIndexCount += 6;
	}

	void Renderer::StartBatch()
	{
		sData.quadIndexCount = 0;
		sData.quadVertexBufferPtr = sData.quadVertexBufferBase;
	}

	void Renderer::Flush()
	{
		if (!sData.quadIndexCount)
			return;

		uint quadDataSize = (uint)((uint8_t*)sData.quadVertexBufferPtr - (uint8_t*)sData.quadVertexBufferBase);
		sData.quadVertexBuffer->setData(sData.quadVertexBufferBase, quadDataSize);

		sData.quadShader->bind();
		DrawBatch();
	}

	void Renderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer::DrawBatch()
	{
		uint32_t count = sData.quadIndexCount ? sData.quadIndexCount : sData.quadVertexArray->getIndexBuffer()->getCount();
		sData.quadVertexArray->bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
#if LAB_DEBUG
		vertexArray->unbind();
#endif
	}
}
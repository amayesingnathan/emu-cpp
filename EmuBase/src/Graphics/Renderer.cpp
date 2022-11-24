#include "glpch.h"
#include "Renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Emulator/Base.h"

#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "PixelBuffer.h"

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

	struct DisplayVertex
	{
		glm::vec3 position;
		glm::vec2 texCoord;
	};

	struct RenderData
	{
		Ref<VertexArray> displayVertexArray;
		Ref<VertexBuffer> displayVertexBuffer;
		Ref<Shader> displayShader;
		Ref<Texture> displayTex = nullptr;

		DisplayVertex* displayVertexBufferBase = nullptr;
		DisplayVertex* displayVertexBufferPtr = nullptr;
	};

	static RenderData sData;

	void Renderer::Init(uint screenWidth, uint screenHeight)
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

		//sData.displayVertexArray = VertexArray::Create();

		//sData.displayVertexBuffer = VertexBuffer::Create(4 * sizeof(DisplayVertex));
		//sData.displayVertexBuffer->setLayout({
		//		{ ShaderDataType::Float3, "aPosition" },
		//		{ ShaderDataType::Float2, "aTexCoord" }
		//	});

		//sData.displayVertexArray->addVertexBuffer(sData.displayVertexBuffer);

		//sData.displayVertexBufferBase = new DisplayVertex[4];

		//uint32_t displayIndices[6];
		//displayIndices[0] = 0;
		//displayIndices[1] = 1;
		//displayIndices[2] = 2;
		//displayIndices[3] = 2;
		//displayIndices[4] = 3;
		//displayIndices[5] = 0;

		//Ref<IndexBuffer> displayIB = IndexBuffer::Create(displayIndices, 6);
		//sData.displayVertexArray->setIndexBuffer(displayIB);

		//sData.displayShader = Shader::Create("shaders/display.glsl");
		sData.displayTex = Texture::Create(screenWidth, screenHeight);
	}

	void Renderer::Shutdown()
	{
		//delete[] sData.displayVertexBufferBase;
	}

	void Renderer::Draw(Ref<PixelBuffer> screenBuffer)
	{
		constexpr glm::vec4 vertexPositions[] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.5f, -0.5f, 0.0f, 1.0f }, { 0.5f,  0.5f, 0.0f, 1.0f }, { -0.5f,  0.5f, 0.0f, 1.0f } };
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		screenBuffer->upload(sData.displayTex);

		BeginFrame();

		for (usize i = 0; i < 4; i++)
		{
			sData.displayVertexBufferPtr->position = vertexPositions[i];
			sData.displayVertexBufferPtr->texCoord = textureCoords[i];
			sData.displayVertexBufferPtr++;
		}

		EndFrame();
	}

	void Renderer::BeginFrame()
	{
		sData.displayVertexBufferPtr = sData.displayVertexBufferBase;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::EndFrame()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)sData.displayVertexBufferPtr - (uint8_t*)sData.displayVertexBufferBase);
		sData.displayVertexBuffer->setData(sData.displayVertexBufferBase, dataSize);

		sData.displayVertexArray->bind();
		sData.displayShader->bind();
		sData.displayTex->bind(0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
}
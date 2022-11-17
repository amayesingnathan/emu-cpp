#include "glpch.h"
#include "Renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "VertexArray.h"

namespace GL {

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

	struct Renderer2DData
	{
		static const uint MaxQuads = 20000;
		static const uint MaxVertices = MaxQuads * 4;
		static const uint MaxIndices = MaxQuads * 6;

		VertexArray* quadVertexArray;
		VertexBuffer* quadVertexBuffer;
		Shader* quadShader;

		uint quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		glm::vec4 quadVertexPositions[4];
	};

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
	}

	void Renderer::Shutdown()
	{
	}
}
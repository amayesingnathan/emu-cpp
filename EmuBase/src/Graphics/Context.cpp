#include "glpch.h"
#include "Context.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Core/Common.h"

namespace Emu {

    GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle)
    {
        glfwMakeContextCurrent(mWindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        GL_ASSERT(status, "Failed to initialise Glad");

        GL_ASSERT(GLVersion.major >= 4, "Require OpenGL version 4 or greater");
    }

    void GraphicsContext::swapBuffers()
    {
        glfwSwapBuffers(mWindowHandle);
    }
}
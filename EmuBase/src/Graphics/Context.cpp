module;

#include "glad/glad.h"
#include "GLFW/glfw3.h"

module EmuBase.Graphics.Context;

import EmuBase.Core.Common;

namespace Emu {

    GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle)
    {
        glfwMakeContextCurrent(mWindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        Assert(status, "Failed to initialise Glad");

        Assert(GLVersion.major >= 4, "Require OpenGL version 4 or greater");
    }

    void GraphicsContext::swapBuffers()
    {
        glfwSwapBuffers(mWindowHandle);
    }
}
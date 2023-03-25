module;

#include "glad/glad.h"
#include "GLFW/glfw3.h"

export module EmuBase.Graphics.Context;

export namespace Emu {

    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle);

        void swapBuffers();

    private:
        GLFWwindow* mWindowHandle;
    };
}
#pragma once

struct GLFWwindow;

namespace Emu {

    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle);

        void swapBuffers();

    private:
        GLFWwindow* mWindowHandle;
    };
}
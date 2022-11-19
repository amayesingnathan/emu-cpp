#pragma once

struct GLFWwindow;

namespace Emu::GL {

    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle);

        void swapBuffers();

    private:
        GLFWwindow* mWindowHandle;
    };
}
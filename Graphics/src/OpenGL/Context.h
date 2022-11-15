#pragma once

struct GLFWwindow;

namespace GL {

    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle)
            : mWindowHandle(windowHandle) {}

        void init();
        void swapBuffers();

    private:
        GLFWwindow* mWindowHandle;
    };
}
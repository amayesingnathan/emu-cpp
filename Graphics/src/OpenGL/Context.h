#pragma once

struct GLFWwindow;

namespace GL {

    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle);

        void swapBuffers();

    private:
        GLFWwindow* mWindowHandle;
    };
}
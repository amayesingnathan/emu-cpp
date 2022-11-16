#pragma once

struct GLFWwindow;

namespace GL {

    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle);

        void init();
        void swapBuffers();

    private:
        GLFWwindow* mWindowHandle;
    };
}
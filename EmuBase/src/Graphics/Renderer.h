#pragma once

#include "Core/Common.h"

namespace GL {

    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

    private:
        static void Flush();
    };
}
#pragma once

#include "glm/glm.hpp"

#include "Core/Common.h"

namespace Emu {

    class PixelBuffer;

    class Renderer
    {
    public:
        static void Init(uint screenWidth, uint screenHeight);
        static void Shutdown();

        static void Draw(Ref<PixelBuffer> screenTex);

    private:
        static void BeginFrame();
        static void EndFrame();
    };
}
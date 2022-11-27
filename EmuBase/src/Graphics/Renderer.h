#pragma once

#include "glm/glm.hpp"

#include "Core/Common.h"

namespace Emu {

    struct Pixel;

    class Renderer
    {
    public:
        static void Init(uint screenWidth, uint screenHeight);
        static void Shutdown();

        static void Clear();
        static void DrawPixels(Pixel* pixels, usize width, usize height);

    private:
        static void BeginFrame();
        static void EndFrame();
    };
}
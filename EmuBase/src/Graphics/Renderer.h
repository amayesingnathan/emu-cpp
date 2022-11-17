#pragma once

#include "glm/glm.hpp"

#include "Core/Common.h"

namespace GL {

    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static void DrawPixel(usize x, usize y, const glm::vec4& colour);

    private:
        static void StartBatch();
        static void Flush();

        static void NextBatch();

        static void DrawBatch();

    private:
        static void DrawPixelImpl(const glm::mat4& transform, const glm::vec4& colour);
    };
}
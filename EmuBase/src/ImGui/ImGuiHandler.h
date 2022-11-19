#pragma once

#include "Core/Common.h"

namespace Emu {
    
    class Window;

    enum class EventType
    {
        Mouse, Keyboard
    };

    class ImGuiHandler
    {
    public:
        ImGuiHandler(Window* window);
        ~ImGuiHandler();

        void update()
        {
            BeginFrame();
            RenderUI();
            EndFrame();
        }

        void setFBO(uint fbo) { mDisplay = fbo; }

        static bool EventHandled(EventType type);

    private:
        void BeginFrame();
        void EndFrame();

        void SetDarkThemeColours();

        void RenderUI();
        void UI_Viewport();

    private:
        uint mDisplay = 0;
        Window* mWindow;
    };
}
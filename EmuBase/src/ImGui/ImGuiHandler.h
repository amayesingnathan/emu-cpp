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

        void begin();
        void end();

        static bool EventHandled(EventType type);

    private:
        void SetDarkThemeColours();

    private:
        Window* mWindow;
    };
}
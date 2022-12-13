#pragma once

#include "Core/Common.h"

namespace Emu {
    
    class Window;
    class Event;

    class ImGuiHandler
    {
    public:
        ImGuiHandler(Window* window);
        ~ImGuiHandler();

        void begin();
        void end();

        void onEvent(Event& event);
        void blockEvents(bool block) { mBlockEvents = block; }

    private:
        void SetDarkThemeColours();

    private:
        Window* mWindow;

        inline static bool mBlockEvents = false;
    };
}
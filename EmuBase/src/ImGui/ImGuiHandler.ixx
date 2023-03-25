export module EmuBase.ImGui.Handler;

import EmuBase.Core.Common;
import EmuBase.IO.Event;
import EmuBase.IO.Window;

namespace Emu {

    export class ImGuiHandler
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
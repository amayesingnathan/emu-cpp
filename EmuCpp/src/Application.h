#pragma once

#include "EmuBase.h"

namespace Emu {

    enum class EmulatorType
    {
        GB
    };

    struct Settings
    {
        EmulatorType type = EmulatorType::GB;
        std::string_view gamePath = "Tetris.gb";
        std::unordered_map<EmulatorType, InputMappings> ioSettings = { { EmulatorType::GB, {} } };
    };

    class Application
    {
    public:
        static void Run();

    private:
        static Application& Get();

    private:
        Application();
        ~Application();

        void RenderUI();
        void UI_MenuBar();
        void UI_Viewport();

    private:
        bool mRunning = true;

        Window* mWindow = nullptr;
        ImGuiHandler* mImGuiHandler = nullptr;

        Base* mGameInstance = nullptr;
        Settings mEmuSettings;
    };
}
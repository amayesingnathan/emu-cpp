#pragma once

#include "EmuBase.h"

namespace Emu {

    enum class EmulatorType
    {
        None, GB
    };

    struct Settings
    {
        EmulatorType type = EmulatorType::None;
        std::string gamePath;
        Duration frameLength = std::chrono::milliseconds(16);
        std::unordered_map<EmulatorType, InputMappings> ioSettings = { { EmulatorType::None, {} }, { EmulatorType::GB, {} } };
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

        void LaunchEmulator();
        void GetROMData(const fs::path& file);

    private:
        bool mRunning = true;

        Window* mWindow = nullptr;
        ImGuiHandler* mImGuiHandler = nullptr;

        Base* mGameInstance = nullptr;
        Settings mEmuSettings;

        LogWidget mLogWidget;
    };
}
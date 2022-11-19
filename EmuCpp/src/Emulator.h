#pragma once

#include "EmuBase.h"

#include "Gameboy.h"

namespace Emu {

    enum class EmulatorType
    {
        GB
    };

    struct Settings
    {
        EmulatorType type = EmulatorType::GB;
        std::string_view gamePath = "Tetris.gb";
        std::unordered_map<EmulatorType, InputMappings> ioSettings;
    };

    class Emulator
    {
    public:
        static void Run()
        {
            Emulator& emu = Get();

            while (emu.mRunning)
            {
                emu.mGameInstance->update();
                emu.mImGuiHandler->update();
                emu.mWindow->update();
            }
        }

    private:
        Emulator()
        {
            Input::Init(mEmuSettings.ioSettings.at(mEmuSettings.type));

            mWindow = new Window();
            Renderer::Init();
            mImGuiHandler = new ImGuiHandler(mWindow);

            switch (mEmuSettings.type)
            {
            case EmulatorType::GB:
                std::filesystem::current_path("../Gameboy");
                mGameInstance = new GB::Gameboy(mEmuSettings.gamePath);
                break;

            default:
                assert(false);
            }

            mImGuiHandler->setFBO(mGameInstance->getFBO());
            mWindow->setActionCallback(mGameInstance->getActionCallback());
        }

        ~Emulator()
        {
            delete mGameInstance;
            delete mImGuiHandler;
            delete mWindow;
        }

        static Emulator& Get()
        {
            static Emulator sInstance;
            return sInstance;
        }
    private:
        bool mRunning = true;

        Window* mWindow = nullptr;
        ImGuiHandler* mImGuiHandler = nullptr;

        Base* mGameInstance = nullptr;
        Settings mEmuSettings;
    };
}
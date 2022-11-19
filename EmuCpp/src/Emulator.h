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

            while (true)
                emu.mGameInstance->update();
        }

    private:
        Emulator()
        {
            mWindow = new Window();
            Renderer::Init();

            switch (mEmuSettings.type)
            {
            case EmulatorType::GB:
                std::filesystem::current_path("../Gameboy");
                mGameInstance = new GB::Gameboy(mEmuSettings.gamePath);
                break;

            default:
                assert(false);
            }
        }

        ~Emulator()
        {
            delete mGameInstance;
            delete mWindow;
        }

        static Emulator& Get()
        {
            static Emulator sInstance;
            return sInstance;
        }
    private:
        Window* mWindow = nullptr;
        Base* mGameInstance = nullptr;
        Settings mEmuSettings;
    };
}
#pragma once

#include "Emulator/Base.h"
#include "Graphics/Renderer.h"
#include "Graphics/Window.h"

#include "Gameboy.h"

enum class EmulatorType
{
    GB
};

struct Settings
{
    EmulatorType type = EmulatorType::GB;
    std::string_view gamePath = "Tetris.gb";
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
        mWindow = new GL::Window();
        GL::Renderer::Init();

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
    GL::Window* mWindow = nullptr;
    Emu::Base* mGameInstance = nullptr;
    Settings mEmuSettings;
};
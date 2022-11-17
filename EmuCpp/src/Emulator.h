#pragma once

#include "Emulator/Base.h"

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
    static void Init()
    {
        if (sGameInstance)
            delete sGameInstance;
                
        switch (sEmuSettings.type)
        {
        case EmulatorType::GB:
            sGameInstance = new GB::Gameboy(sEmuSettings.gamePath);
            break;

        default:
            assert(false);
        }
    }

    static void Run()
    {
        while (true)
            sGameInstance->update();
    }

    static void Shutdown()
    {
        delete sGameInstance;
    }

private:
    inline static Emu::Base* sGameInstance = nullptr;
    inline static Settings sEmuSettings;
};
#pragma once

#include "Gameboy.h"

struct Settings
{
    std::string_view gamePath = "Tetris.gb";
};

class Emulator
{
public:
    static void Init()
    {
        if (sGameInstance)
            delete sGameInstance;
                
        sGameInstance = new GB::Gameboy(sEmuSettings.gamePath);
    }

    static void Run()
    {
        while (true)
        {
            sGameInstance->update();
        }
    }

private:
    inline static GB::Gameboy* sGameInstance = nullptr;
    inline static Settings sEmuSettings;
};
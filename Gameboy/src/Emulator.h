#pragma once

#include "Gameboy.h"

namespace GB {

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
                
            sGameInstance = new Gameboy(sEmuSettings.gamePath);
        }

        static void Run()
        {
            while (true)
            {
                sGameInstance->update();
            }
        }

    private:
        inline static Gameboy* sGameInstance = nullptr;
        inline static Settings sEmuSettings;
    };
}
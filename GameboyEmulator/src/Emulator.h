#pragma once

#include "Gameboy.h"

namespace GB {

    struct Settings
    {
        std::string_view gamePath;
    };

    class Emulator
    {
    public:
        static void Init()
        {

        }

        static void Run()
        {
            if (sGameInstance)
                delete sGameInstance;

            sGameInstance = new Gameboy(sEmuSettings.gamePath);

            while (true)
            {

            }
        }

    private:
        inline static Gameboy* sGameInstance = nullptr;
        inline static Settings sEmuSettings;
    };
}
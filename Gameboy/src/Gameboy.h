#pragma once

#include "Emulator/Base.h"

#include "CPU/CPU.h"
#include "Graphics/PPU.h"
#include "Cartridge/Cartridge.h"

namespace GB {

    struct APU {};

    class Gameboy : public Emu::Base
    {
    public:
        GB_CONST USize _MaxCycles = 69905;

    public:
        Gameboy(std::string_view path);
        ~Gameboy() override;

    public:
        void update() override;

    private:
        void UpdateGraphics(int cycles)
        {
        }

        void Interupts()
        {
        }

        void Render()
        {

        }

    private:
        Cartridge* mCartridge = nullptr;

        CPU mProcessor;
        PPU mGraphics;
        APU mAudio;
    };
}
#pragma once

#include "CPU/CPU.h"
#include "Cartridge/Cartridge.h"

namespace GB {

    struct PPU { Byte mVRAM[0x2000]; };
    struct APU {};

    class Gameboy
    {
    public:
        GB_CONST USize _MaxCycles = 69905;

    public:
        Gameboy(std::string_view path)
            : mCartridge(path) {}

    public:
        void update()
        {
            int cyclesThisUpdate = 0;

            while (cyclesThisUpdate < _MaxCycles)
            {
                int cycles = mProcessor.exec();
                cyclesThisUpdate += cycles;
                // UpdateTimers(cycles);
                // UpdateGraphics(cycles);
                // DoInterupts();
            }
            //RenderScreen();
        }

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
        Cartridge mCartridge;

        CPU mProcessor;
        PPU mGraphics;
        APU mAudio;
    };
}
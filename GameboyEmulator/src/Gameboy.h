#pragma once

#include "CPU/CPU.h"
#include "Cartridge/Cartridge.h"
#include "Memory/WRAM.h"

namespace GB {

    struct PPU { Byte mVRAM[0x2000]; };
    struct APU {};

    class Gameboy
    {
    public:

    private:
        CPU mProcessor;
        PPU mGraphics;
        APU mAudio;

        Cartridge mCartridge;
        WRAM mMainMemory;

        friend class MemoryMap;
    };
}
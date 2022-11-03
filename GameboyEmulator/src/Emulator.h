#pragma once

#include "Memory/Memory.h"

namespace GB {

    class Emulator
    {
    public:
        GB_CONST USize _MaxCycles = 69905;
        GB_CONST USize _ROMSize = 0x10000;

    public:
        Emulator()
        {
        }

        ~Emulator()
        {
        }

    public:
        void update()
        {
            int cycleCount = 0;
            while (cycleCount < _MaxCycles)
            {
                int cycles = ExecNextOpcode();
                cycleCount += cycles;

                mProcessor.updateTimers(cycles);
                UpdateGraphics(cycles);
                Interupts();
            }

            Render();
        }

    private:
        Byte Read(Word address) const { return mProcessor}

    private:
        int ExecNextOpcode()
        {
        }

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
        CPU mProcessor;
    };
}
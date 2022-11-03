#pragma once

#include "Emulator/Memory.h"

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
            USize cycleCount = 0;
            while (cycleCount < _MaxCycles)
            {
                USize cycles = ExecNextOpcode();
                cycleCount += cycles;

                mMemory.updateTimers(cycles);
                UpdateGraphics(cycles);
                Interupts();
            }

            Render();
        }

    private:
        USize ExecNextOpcode()
        {
        }

        void UpdateGraphics(USize cycles)
        {
        }

        void Interupts()
        {
        }

        void Render()
        {

        }

    private:
        Word mProgramCounter = 0x100;
        Register mStackPointer = 0xFFFE;
        Memory mMemory;
    };
}
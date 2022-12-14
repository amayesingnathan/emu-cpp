#pragma once

#include "Core/Types.h"

#include "MBC.h"

namespace GB {

    class Cartridge
    {
    public:
        GB_CONST USize ROM_MAX_SIZE = 0x200000;
        GB_CONST USize RAM_MAX_SIZE = 0x20000;

    public:
        Cartridge(std::string_view filename);
        ~Cartridge();

    public:
        Byte& read(Word address);
        void write(Word address, Byte data);

    private:
        Byte* mROM = nullptr;
        Byte* mRAM = nullptr;
        MBC* mMBC = nullptr;
    };
}
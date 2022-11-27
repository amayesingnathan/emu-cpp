#pragma once

#include "Core/Types.h"

namespace GB {

    GB_CONST Byte _CarryBit = 4;
    GB_CONST Byte _HCarryBit = 5;
    GB_CONST Byte _SubtractBit = 6;
    GB_CONST Byte _ZeroBit = 7;

    class FRegister
    {
    private:
        Byte& reg;

    public:
        FRegister(Byte& _reg) : reg(_reg) {}

        bool carry() const { BitField flags = reg; return flags.bit(_CarryBit); }
        bool hcarry() const { BitField flags = reg; return flags.bit(_HCarryBit); }
        bool subtr() const { BitField flags = reg; return flags.bit(_SubtractBit); }
        bool zero() const { BitField flags = reg; return flags.bit(_ZeroBit); }

        BitField getFlags() const { return reg; }
        void setFlags(BitField flag) { flag &= 0xF0; reg = flag; }
    };
}
#pragma once

#include "Core/Flag.h"

namespace GB {

    GB_CONST Byte _CarryBit = 4;
    GB_CONST Byte _HCarryBit = 5;
    GB_CONST Byte _SubtractBit = 6;
    GB_CONST Byte _ZeroBit = 7;

    class FRegister
    {
    private:
        BitField reg;

    public:
        FRegister(Byte& _reg) : reg(_reg) {}

        bool carry() const { return reg.bit(_CarryBit); }
        bool hcarry() const { return reg.bit(_HCarryBit); }
        bool subtr() const { return reg.bit(_SubtractBit); }
        bool zero() const { return reg.bit(_ZeroBit); }

        BitField& getFlags() { return reg; }
        const BitField& getFlags() const { return reg; }
        void setFlags(BitField flag) { flag &= 0xF0; reg = flag; }
    };
}
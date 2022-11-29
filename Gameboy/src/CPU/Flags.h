#pragma once

#include "Core/Types.h"

namespace GB {

    GB_CONST Byte CARRY_BIT      = 4;
    GB_CONST Byte H_CARRY_BIT    = 5;
    GB_CONST Byte SUBTRACT_BIT   = 6;
    GB_CONST Byte ZERO_BIT       = 7;

    class FRegister
    {
    private:
        Byte& reg;

    public:
        FRegister(Byte& _reg) : reg(_reg) {}

        bool carry() const { BitField flags = reg; return flags.bit(CARRY_BIT); }
        bool hcarry() const { BitField flags = reg; return flags.bit(H_CARRY_BIT); }
        bool subtr() const { BitField flags = reg; return flags.bit(SUBTRACT_BIT); }
        bool zero() const { BitField flags = reg; return flags.bit(ZERO_BIT); }

        BitField getFlags() const { return reg; }
        void setFlags(BitField flag) { flag &= 0xF0; reg = flag; }
    };
}
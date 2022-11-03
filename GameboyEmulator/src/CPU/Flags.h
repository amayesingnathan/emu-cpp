#pragma once

#include "Core/Types.h"

namespace GB {

    using Flag = Byte;
    GB_CONST Flag _CarryFlag = 1 << 4;
    GB_CONST Flag _HCarryFlag = 1 << 5;
    GB_CONST Flag _SubtractFlag = 1 << 6;
    GB_CONST Flag _ZeroFlag = 1 << 7;

    class FlagsRegister
    {
    private:
        Byte& reg;

    public:
        FlagsRegister(Byte& _reg) : reg(_reg) {}

        inline bool carry() const { return reg & _CarryFlag; }
        inline bool hcarry() const { return reg & _HCarryFlag; }
        inline bool subtr() const { return reg & _SubtractFlag; }
        inline bool zero() const { return reg & _ZeroFlag; }

        inline void setFlags(Flag flag) { flag &= 0xF0; reg |= flag; }
    };
}
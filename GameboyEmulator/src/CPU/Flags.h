#pragma once

#include "Core/Types.h"

namespace GB {

    GB_CONST Flag _CarryFlag = 1 << 4;
    GB_CONST Flag _HCarryFlag = 1 << 5;
    GB_CONST Flag _SubtractFlag = 1 << 6;
    GB_CONST Flag _ZeroFlag = 1 << 7;

    class FRegister
    {
    private:
        Byte& reg;

    public:
        FRegister(Byte& _reg) : reg(_reg) {}

        bool carry() const { return reg & _CarryFlag; }
        bool hcarry() const { return reg & _HCarryFlag; }
        bool subtr() const { return reg & _SubtractFlag; }
        bool zero() const { return reg & _ZeroFlag; }

        Flag getFlags() const { return reg; }
        void setFlags(Flag flag) { flag &= 0xF0; reg = flag; }
    };
}
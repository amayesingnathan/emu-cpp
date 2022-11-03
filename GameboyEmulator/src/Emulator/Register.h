#pragma once

#include "Core/Types.h"

namespace GB {
    
    union Register
    {
        Word reg = 0;

        struct { Byte hi, lo; };

        Register(Word val) : reg(val) {}
    };

    enum class Reg
    {
        A, F, B, C, D, E, H, L,
        AF, BC, DE, HL
    };

    using Flag = Byte;
    constexpr Flag _CarryFlag = 4;
    constexpr Flag _HCarryFlag = 5;
    constexpr Flag _SubtractFlag = 6;
    constexpr Flag _ZeroFlag = 7;

    class Registers
    {
    private:
        union InternalRegister
        {
            union
            {
                struct { Byte A, F, B, C, D, E, H, L; };

                struct { Word AF, BC, DE, HL; };
            };

            Register registers[4];

            InternalRegister()
            {
                AF = 0x01B0;
                BC = 0x0013;
                DE = 0x00D8;
                HL = 0x014D;
            };
        };

    public:
        Registers() = default;

    public:
        Byte& operator[](Reg reg)
        {
            switch (reg)
            {
            case Reg::A: return mRegisters.A;
            case Reg::F: return mRegisters.F;
            case Reg::B: return mRegisters.B;
            case Reg::C: return mRegisters.C;
            case Reg::D: return mRegisters.D;
            case Reg::E: return mRegisters.E;
            case Reg::H: return mRegisters.H;
            case Reg::L: return mRegisters.L;
            }

            GB_ASSERT(false, "Wrong access type!");
            return mRegisters.A;
        }
        const Byte& operator[](Reg reg) const
        {
            switch (reg)
            {
            case Reg::A: return mRegisters.A;
            case Reg::F: return mRegisters.F;
            case Reg::B: return mRegisters.B;
            case Reg::C: return mRegisters.C;
            case Reg::D: return mRegisters.D;
            case Reg::E: return mRegisters.E;
            case Reg::H: return mRegisters.H;
            case Reg::L: return mRegisters.L;
            }

            GB_ASSERT(false, "Wrong access type!");
            return mRegisters.A;
        }

        Word& operator()(Reg reg)
        {
            switch (reg)
            {
            case Reg::AF: return mRegisters.AF;
            case Reg::BC: return mRegisters.BC;
            case Reg::DE: return mRegisters.DE;
            case Reg::HL: return mRegisters.HL;
            }

            GB_ASSERT(false, "Wrong access type!");
            return mRegisters.AF;
        }
        const Word& operator()(Reg reg) const
        {
            switch (reg)
            {
            case Reg::AF: return mRegisters.AF;
            case Reg::BC: return mRegisters.BC;
            case Reg::DE: return mRegisters.DE;
            case Reg::HL: return mRegisters.HL;
            }

            GB_ASSERT(false, "Wrong access type!");
            return mRegisters.AF;
        }

    private:
        InternalRegister mRegisters;
    };

}
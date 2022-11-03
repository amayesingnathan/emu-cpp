#pragma once

#include "Core/Types.h"

namespace GB {
    
    union Register
    {
        Word dreg = 0;

        struct { Byte hi, lo; };

        Register() = default;
        Register(Word val) : dreg(val) {}

        Register& operator++() 
        { 
            ++dreg;
            return *this; 
        }
        Register operator++(int)
        {
            Register tmp = *this;
            ++(*this);
            return tmp;
        }
        Register& operator--()
        {
            --dreg;
            return *this;
        }
        Register operator--(int)
        {
            Register tmp = *this;
            --(*this);
            return tmp;
        }
    };

    enum class ByteReg
    {
        A, F, B, C, D, E, H, L
    };
    enum class WordReg
    {
        AF, BC, DE, HL
    };

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
        Byte& operator[](ByteReg reg)
        {
            switch (reg)
            {
                case ByteReg::A: return mRegisters.A;
                case ByteReg::F: return mRegisters.F;
                case ByteReg::B: return mRegisters.B;
                case ByteReg::C: return mRegisters.C;
                case ByteReg::D: return mRegisters.D;
                case ByteReg::E: return mRegisters.E;
                case ByteReg::H: return mRegisters.H;
                case ByteReg::L: return mRegisters.L;
                default: GB_ASSERT(false, "Wrong access type!");
            }
        }
        const Byte& operator[](ByteReg reg) const
        {
            switch (reg)
            {
            case ByteReg::A: return mRegisters.A;
            case ByteReg::F: return mRegisters.F;
            case ByteReg::B: return mRegisters.B;
            case ByteReg::C: return mRegisters.C;
            case ByteReg::D: return mRegisters.D;
            case ByteReg::E: return mRegisters.E;
            case ByteReg::H: return mRegisters.H;
            case ByteReg::L: return mRegisters.L;
            default: GB_ASSERT(false, "Wrong access type!");
            }
        }

        Word& operator[](WordReg reg)
        {
            switch (reg)
            {
            case WordReg::AF: return mRegisters.AF;
            case WordReg::BC: return mRegisters.BC;
            case WordReg::DE: return mRegisters.DE;
            case WordReg::HL: return mRegisters.HL;
            default: GB_ASSERT(false, "Wrong access type!");
            }
        }
        const Word& operator[](WordReg reg) const
        {
            switch (reg)
            {
            case WordReg::AF: return mRegisters.AF;
            case WordReg::BC: return mRegisters.BC;
            case WordReg::DE: return mRegisters.DE;
            case WordReg::HL: return mRegisters.HL;
            default: GB_ASSERT(false, "Wrong access type!");
            }
        }
    private:
        InternalRegister mRegisters;
    };

}
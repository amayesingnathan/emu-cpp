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
        AF, BC, DE, HL, SP, PC
    };

    class Registers
    {
    private:
        struct InternalRegister
        {
            union
            {
                struct { Byte A, F, B, C, D, E, H, L, SP1, SP2, PC1, PC2; };

                struct { Word AF, BC, DE, HL, SP, PC; };
            };

            InternalRegister()
            {
                AF = 0x01B0;
                BC = 0x0013;
                DE = 0x00D8;
                HL = 0x014D;
                SP = 0xFFFE;
                PC = 0x100;
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
            case WordReg::SP: return mRegisters.SP;
            case WordReg::PC: return mRegisters.PC;
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
            case WordReg::SP: return mRegisters.SP;
            case WordReg::PC: return mRegisters.PC;
            }
        }
    private:
        InternalRegister mRegisters;
    };

}
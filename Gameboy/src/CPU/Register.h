#pragma once

#include "Core/Types.h"

namespace GB {

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
                struct { Word AF, BC, DE, HL, SP, PC; };
                struct { Byte F_L, A_L, C_L, B_L, E_L, D_L, L_L, H_L, SP1_L, SP2_L, PC1_L, PC2_L; };
                struct { Byte A_B, F_B, B_B, C_B, D_B, E_B, H_B, L_B, SP1_B, SP2_B, PC1_B, PC2_B; };
            };

            InternalRegister()
            {
                AF = 0x0000;
                BC = 0x0000;
                DE = 0x0000;
                HL = 0x0000;
                SP = 0x0000;
                PC = 0x0000;
            };
        };

    public:
        Registers() = default;

    public:
        Byte& operator[](ByteReg reg)
        {
            if_c (GB_BIG_ENDIAN)
            {
                switch (reg)
                {
                case ByteReg::A: return mRegisters.A_B;
                case ByteReg::F: return mRegisters.F_B;
                case ByteReg::B: return mRegisters.B_B;
                case ByteReg::C: return mRegisters.C_B;
                case ByteReg::D: return mRegisters.D_B;
                case ByteReg::E: return mRegisters.E_B;
                case ByteReg::H: return mRegisters.H_B;
                case ByteReg::L: return mRegisters.L_B;
                }
            }
            else if_c (GB_LITTLE_ENDIAN)
            {
                switch (reg)
                {
                case ByteReg::A: return mRegisters.A_L;
                case ByteReg::F: return mRegisters.F_L;
                case ByteReg::B: return mRegisters.B_L;
                case ByteReg::C: return mRegisters.C_L;
                case ByteReg::D: return mRegisters.D_L;
                case ByteReg::E: return mRegisters.E_L;
                case ByteReg::H: return mRegisters.H_L;
                case ByteReg::L: return mRegisters.L_L;
                }
            }

            GB_ASSERT(false, "Invalid parameter!");
            return mRegisters.A_L;
        }
        Byte operator[](ByteReg reg) const
        {
            if_c (GB_BIG_ENDIAN)
            {
                switch (reg)
                {
                case ByteReg::A: return mRegisters.A_B;
                case ByteReg::F: return mRegisters.F_B;
                case ByteReg::B: return mRegisters.B_B;
                case ByteReg::C: return mRegisters.C_B;
                case ByteReg::D: return mRegisters.D_B;
                case ByteReg::E: return mRegisters.E_B;
                case ByteReg::H: return mRegisters.H_B;
                case ByteReg::L: return mRegisters.L_B;
                }
            }
            else if_c (GB_LITTLE_ENDIAN)
            {
                switch (reg)
                {
                case ByteReg::A: return mRegisters.A_L;
                case ByteReg::F: return mRegisters.F_L;
                case ByteReg::B: return mRegisters.B_L;
                case ByteReg::C: return mRegisters.C_L;
                case ByteReg::D: return mRegisters.D_L;
                case ByteReg::E: return mRegisters.E_L;
                case ByteReg::H: return mRegisters.H_L;
                case ByteReg::L: return mRegisters.L_L;
                }
            }

            GB_ASSERT(false, "Invalid parameter!");
            return mRegisters.A_L;
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

            GB_ASSERT(false, "Invalid parameter!");
            return mRegisters.AF;
        }
        Word operator[](WordReg reg) const
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

            GB_ASSERT(false, "Invalid parameter!");
            return mRegisters.AF;
        }
    private:
        InternalRegister mRegisters;
    };

}
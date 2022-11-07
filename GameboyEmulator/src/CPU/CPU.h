#pragma once

#include "Memory/AddressBus.h"

#include "Register.h"
#include "Bootstrap.h"
#include "Flags.h"
#include "WRAM.h"
#include "HRAM.h"

namespace GB {

    struct OpCode;

    enum class Condition : Byte
    {
        NZ = 0, Z, NC, C,
    };

	class CPU
	{
	public:
		CPU()
			: mFRegister(mRegisters[ByteReg::F])
		{

		}

    public:
        int exec();

    private:
        bool CheckCondition(Condition condition);

        int HandleInstruction(OpCode instruction);
        int HandleCBInstruction();

	private:
        // Registers
		Registers mRegisters;
        FRegister mFRegister;

        // Accessible through AddressBus only
        BootROM mBootROM;
        MainMemory mMainMemory;
        StackMemory mStack;

        // Flags
        bool mInteruptsEnabled = false;
        bool mHalted = false;

        bool mBranchTaken = false;

#pragma region OpCodeFunctions
    private:
        void LD_R(OpCode op);
        void LD_M_R(OpCode op);

        void ALU_R(OpCode op);

        void ROT_R(OpCode op);
        void BIT_R(OpCode op);
        void RES_R(OpCode op);
        void SET_R(OpCode op);

        void JR_C8(OpCode op);

    private:
        void ADD_R(Byte target);
        void ADC_R(Byte target);
        void SUB_R(Byte target);
        void SBC_R(Byte target);
        void AND_R(Byte target);
        void XOR_R(Byte target);
        void OR_R(Byte target);
        void CP_R(Byte target);

        void RLC_R(Byte target);
        void RRC_R(Byte target);
        void RL_R(Byte target);
        void RR_R(Byte target);
        void SLA_R(Byte target);
        void SRA_R(Byte target);
        void SWAP_R(Byte target);
        void SRL_R(Byte target);

        void PUSH(Byte target);
        void POP(Byte target);
#pragma endregion
	
    };
}
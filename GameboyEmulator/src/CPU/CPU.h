#pragma once

#include "Memory/AddressBus.h"

#include "Register.h"
#include "Bootstrap.h"
#include "Flags.h"
#include "WRAM.h"

namespace GB {

    struct OpCode;

    enum class Condition {
        NZ, Z, NC, C,
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
        bool IsCondition(Condition condition);

        int HandleInstruction(OpCode instruction);
        int HandleCBInstruction(OpCode instruction);

	private:
		Registers mRegisters;
        FRegister mFRegister;

        MainMemory mMainMemory;
        BootROM mBootROM;

        bool mInteruptsEnabled = false;
        bool mHalted = false;

        bool mBranchTaken = false;

#pragma region OpCodeFunctions
    private:
        void LD_R(OpCode op);
        void ALU_R(OpCode op);

    private:
        void ADD_R(Byte target);
        void ADC_R(Byte target);
        void SUB_R(Byte target);
        void SBC_R(Byte target);
        void AND_R(Byte target);
        void XOR_R(Byte target);
        void OR_R(Byte target);
        void CP_R(Byte target);
#pragma endregion
	
    };
}
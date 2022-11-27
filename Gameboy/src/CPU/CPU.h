#pragma once

#include "Memory/AddressBus.h"

#include "Register.h"
#include "Flags.h"

namespace GB {

    struct OpCode;

    enum class Condition : Byte
    {
        NZ = 0, Z, NC, C,
    };

	class CPU
	{
    public:
        GB_CONST USize _ClockSpeed = 4194304;
        GB_CONST Word _TMC0 = _ClockSpeed / 4096;
        GB_CONST Word _TMC1 = _ClockSpeed / 262144;
        GB_CONST Word _TMC2 = _ClockSpeed / 65536;
        GB_CONST Word _TMC3 = _ClockSpeed / 16382;

        enum TimerControl : Word
        {
            TMC0, TMC1, TMC2, TMC3
        };

	public:
		CPU()
			: mFRegister(mRegisters[ByteReg::F]) {}

    public:
        Byte exec();
        void updateTimers(Byte cycles);
        void handleInterupts();

        Word& getClockSpeed() { return mCurrentClockSpeed; }

    private:
        bool CheckCondition(Condition condition);

        Byte HandleInstruction(OpCode instruction);
        Byte HandleCBInstruction();

        void ServiceInterupt(Interrupt interrupt);

        bool IsClockEnabled();
        Byte GetClockFreq();
        void SetClockFreq();

	private:
        // Registers
		Registers mRegisters;
        FRegister mFRegister;

        Word mDividerClock = 0;
        Word mTimerClock = 0;

        Word mCurrentClockSpeed = _TMC0;

        // Flags
        bool mInteruptsEnabled = false;
        bool mHalted = false;

        bool mBranchTaken = false;

        friend class Gameboy;

#pragma region OpCodeFunctions
    private:
        void LD_R_R(OpCode op);
        void LD_M_R(OpCode op);
        void LD_R_M(OpCode op);
        void LD_R_I8(OpCode op);
        void LD_R_STK(OpCode op);
        void LD_R_STK16(OpCode op);

        void ROT_R(OpCode op);
        void BIT_R(OpCode op);
        void RES_R(OpCode op);
        void SET_R(OpCode op);

        void JR_C8(OpCode op);
        void JR_C16(OpCode op);

        void UN_R(OpCode op);

        void CALL(OpCode op);
        void RET(OpCode op);

        void RST(OpCode op);

        void REG(OpCode op);

    private:
        // Instruction Implementations
        void ALU_T(Byte target, Byte src);
        void ADD_R(Byte value);
        void ADC_R(Byte value);
        void SUB_R(Byte value);
        void SBC_R(Byte value);
        void AND_R(Byte value);
        void XOR_R(Byte value);
        void OR_R(Byte value);
        void CP_R(Byte value);

        void ADD_HL(Word value);

        void PUSH(Byte target);
        void POP(Byte target);

        void INC(Byte target);
        void DEC(Byte target);
        void INC16(Byte target);
        void DEC16(Byte target);

        void RLCA();
        void RRCA();
        void RLA();
        void RRA();
        void DAA();
        void CPL();
        void SCF();
        void CCF();

        // CB Instructions
        void RLC_R(Byte target);
        void RRC_R(Byte target);
        void RL_R(Byte target);
        void RR_R(Byte target);
        void SLA_R(Byte target);
        void SRA_R(Byte target);
        void SWAP_R(Byte target);
        void SRL_R(Byte target);
#pragma endregion
	
    };
}
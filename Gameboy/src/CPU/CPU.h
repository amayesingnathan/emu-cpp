#pragma once

#include "Memory/AddressBus.h"

#include "Register.h"
#include "Flags.h"
#include "OpCode.h"

namespace GB {

    struct OpCode;

    enum class Condition : Byte
    {
        NZ = 0, Z, NC, C,
    };

	class CPU
	{
    public:
        GB_CONST USize CLOCK_SPEED  = 4194304;
        GB_CONST Word TMC0          = CLOCK_SPEED / 4096;
        GB_CONST Word TMC1          = CLOCK_SPEED / 262144;
        GB_CONST Word TMC2          = CLOCK_SPEED / 65536;
        GB_CONST Word TMC3          = CLOCK_SPEED / 16382;

        enum class Timer : Word
        {
            TMC0, TMC1, TMC2, TMC3
        };

	public:
        CPU();

    public:
        Byte exec();
        Byte execDebug(Word breakpoint, bool& pause, bool& step);
        void updateTimers(Byte cycles);
        void handleInterupts();

        Word& getClockSpeed() { return mCurrentClockSpeed; }

    private:
        void InitDispatcher();

        void DividerTimer();

        bool CheckCondition(Condition condition);

        Byte HandleInstruction(OpCode instruction);

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

        Word mCurrentClockSpeed = TMC0;

        // Flags
        bool mInterruptsEnabled = false;
        bool mHalted = false;

        bool mBranchTaken = false;
        bool mCBInstruction = false;

        using OpcodeFunction = std::function<void()>;
        OpcodeFunction mDispatcher[0x100];

        friend class Gameboy;

#pragma region OpCodeFunctions
    private:
        void LD_R_R(OpCode op);
        void LD_M_R(OpCode op);
        void LD_R_M(OpCode op);
        void LD_R_I8(OpCode op);
        void LD_R_I16(OpCode op);
        void LD_R_STK(OpCode op);
        void LD_R_STK16(OpCode op);
        void LD_HL_PC(OpCode) { mRegisters[WordReg::PC] = mRegisters[WordReg::HL]; }

        void LD_M_SP(OpCode op);

        void ROT_R(OpCode op);
        void BIT_R(OpCode op);
        void RES_R(OpCode op);
        void SET_R(OpCode op);

        void ADD_HL(OpCode op);

        void JP_C8(OpCode op);
        void JP_C16(OpCode op);
        void JP_I16(OpCode op);

        void UN_R(OpCode op);

        void CALL(OpCode op);
        void RET(OpCode op);
        void RETI(OpCode op) { RET(op); mInterruptsEnabled = true; }

        void PUSH(OpCode op);
        void POP(OpCode op);

        void RST(OpCode op);

        void REG(OpCode op);

        void EI(OpCode op) { mInterruptsEnabled = true; }
        void DI(OpCode op) { mInterruptsEnabled = false; }

        void HALT(OpCode op) { mHalted = true; }
        void STOP(OpCode op) { mRegisters[WordReg::PC]++; }

        void CB(OpCode op);

        void ASSERT(OpCode op) { GB_ASSERT(false, "Instruction not implemented!"); }

    private:
        // Instruction Implementations
        void NOP(OpCode opcode) {}

        void ALU_T(OpCode opcode);
        void ADD_R(Byte value);
        void ADC_R(Byte value);
        void SUB_R(Byte value);
        void SBC_R(Byte value);
        void AND_R(Byte value);
        void XOR_R(Byte value);
        void OR_R(Byte value);
        void CP_R(Byte value);

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
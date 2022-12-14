#pragma once

#include "Memory/AddressBus.h"

#include "Register.h"
#include "Flags.h"
#include "OpCode.h"

namespace GB {

    struct OpCode;
    struct GBSession;

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

        enum class Timer : Byte
        {
            TMC0, TMC1, TMC2, TMC3
        };

	public:
        CPU(GBSession* session);

    public:
        Byte exec();
        void updateTimers(Byte cycles);
        void handleInterupts();

        void postBoot();
        Word& getClockSpeed() { return mCurrentClockSpeed; }

    private:
        void InitDispatcher();

        Byte ExecDebug();
        void DividerTimer();
        void ServiceInterupt(Interrupt interrupt);

        Byte HandleInstruction(OpCode instruction);
        bool CheckCondition(Condition condition);

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

        GBSession* mSession = nullptr;

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
        void RETI(OpCode op);

        void PUSH(OpCode op);
        void POP(OpCode op);

        void RST(OpCode op);

        void REG(OpCode op);

        void EI(OpCode op);
        void DI(OpCode op);

        void HALT(OpCode op);
        void STOP(OpCode op);

        void CB(OpCode op);

        void ASSERT(OpCode op) { GB_ASSERT(false, "Instruction not implemented!"); }

    private:
        // Instruction Implementations
        void NOP(OpCode opcode);

        void ALU_T(OpCode opcode);
        void ADD_R(OpCode opcode);
        void ADC_R(OpCode opcode);
        void SUB_R(OpCode opcode);
        void SBC_R(OpCode opcode);
        void AND_R(OpCode opcode);
        void XOR_R(OpCode opcode);
        void OR_R(OpCode opcode);
        void CP_R(OpCode opcode);

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
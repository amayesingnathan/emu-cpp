#include "gbpch.h"
#include "CPU.h"

#include "Memory/Address.h"

#include "Gameboy.h"
#include "Instructions.h"

namespace GB {

	CPU::CPU(GBSession* session)
		: mFRegister(mRegisters[ByteReg::F]), mSession(session)
	{
		InitDispatcher();
	}

	Byte CPU::exec()
	{
		if (mSession->useBreakpoint)
			return ExecDebug();

		if (mHalted)	
			return 1;

		Word& pc = mRegisters[WordReg::PC];
		mSession->lastPC = pc;

		OpCode nextOp = AddressBus::Read(pc++);
		return HandleInstruction(nextOp) * 4;
	}

	Byte CPU::ExecDebug()
	{
		if (mHalted)
			return 1;

		Word& pc = mRegisters[WordReg::PC];
		mSession->lastPC = pc;
		if (pc == mSession->breakpoint || (pc > mSession->breakpoint && (pc < mSession->breakpoint + 5)) )
		{
			mSession->paused = true;
			mSession->step = true;
			return 0;
		}

		OpCode nextOp = AddressBus::Read(pc++);
		return HandleInstruction(nextOp) * 4;
	}

	void CPU::updateTimers(Byte cycles)
	{
		mDividerClock += cycles;

		if (!IsClockEnabled())
		{
			DividerTimer();
			return;
		}

		mTimerClock += cycles;
		if (mTimerClock < mCurrentClockSpeed)
		{
			DividerTimer();
			return;
		}

		mTimerClock -= mCurrentClockSpeed;

		Byte& timerCounter = AddressBus::Read(Addr::TIMA);
		if (timerCounter < 0xFF)
		{
			timerCounter++;
			DividerTimer();
			return;
		}

		timerCounter = AddressBus::Read(Addr::TMA);
		AddressBus::RequestInterrupt(Interrupt::TIMER);

		DividerTimer();
	}

	void CPU::handleInterupts()
	{
		if (!mInterruptsEnabled)
			return;

		BitField req = AddressBus::Read(Addr::IF);
		BitField enabled = AddressBus::Read(Addr::IE);

		if (req == 0)
			return;

		for (Byte i = 0; i < 5; i++)
		{
			if (!req.bit(i))
				continue;

			if (!enabled.bit(i))
				continue;

			ServiceInterupt((Interrupt)i);
		}
	}

	void CPU::DividerTimer()
	{
		if (mDividerClock < 0x100)
			return;

		mDividerClock -= 0x100;
		AddressBus::Read(Addr::DIV)++;
	}

	bool CPU::CheckCondition(Condition condition)
	{
		switch (condition) {
		case Condition::C:
			mBranchTaken = mFRegister.carry();
			break;
		case Condition::NC:
			mBranchTaken = !mFRegister.carry();
			break;
		case Condition::Z:
			mBranchTaken = mFRegister.zero();
			break;
		case Condition::NZ:
			mBranchTaken = !mFRegister.zero();
			break;
		}

		return mBranchTaken;
	}

	Byte CPU::HandleInstruction(OpCode instruction)
	{
		mBranchTaken = false;
		mCBInstruction = false;

		mDispatcher[instruction]();

		EMU_TRACE("IF: 0x{0:02X}", AddressBus::Read(Addr::IF));

		if (mCBInstruction)
			return sCycles[0xCB] + sCBCycles[instruction];

		if (mBranchTaken)
			return sCyclesBranched[instruction];

		return sCycles[instruction];
	}

	void CPU::ServiceInterupt(Interrupt interrupt)
	{
		mInterruptsEnabled = false;
		Byte& interruptFlag = AddressBus::Read(Addr::IF);
		BitField ifBitField = interruptFlag;
		ifBitField.reset((Byte)interrupt);
		interruptFlag = ifBitField;

		Word& pc = mRegisters[WordReg::PC];
		Word& sp = mRegisters[WordReg::SP];
		AddressBus::Write(--sp, (Byte)(pc >> 8));
		AddressBus::Write(--sp, (Byte)(pc & 0x00FF));

		switch (interrupt)
		{
		case Interrupt::VBLANK:
			pc = 0x40;
			break;

		case Interrupt::LCD_STAT:
			pc = 0x48;
			break;

		case Interrupt::TIMER:
			pc = 0x50;
			break;

		case Interrupt::SERIAL:
			pc = 0x58;
			break;

		case Interrupt::JOYPAD:
			pc = 0x60;
			break;

		default:
			GB_ASSERT(false);
			break;
		}
	}

	bool CPU::IsClockEnabled()
	{
		BitField timerController = AddressBus::Read(Addr::TMC);
		return timerController.bit(2);
	}

	Byte CPU::GetClockFreq()
	{
		return AddressBus::Read(Addr::TMC) & 0x3;
	}

	void CPU::SetClockFreq()
	{
		Timer freq = (Timer)GetClockFreq();
		switch (freq)
		{
		case Timer::TMC0:
			mTimerClock = TMC0;
			break;

		case Timer::TMC1:
			mTimerClock = TMC1;
			break;

		case Timer::TMC2:
			mTimerClock = TMC2;
			break;

		case Timer::TMC3:
			mTimerClock = TMC3;
			break;
		}
	}

#pragma region DispatcherInit
	void CPU::InitDispatcher()
	{
		mDispatcher[0x00] = GB_BIND_DISPATCH(NOP, 0x00);		mDispatcher[0x10] = GB_BIND_DISPATCH(STOP, 0x10);			mDispatcher[0x20] = GB_BIND_DISPATCH(JP_C8, 0x20);			mDispatcher[0x30] = GB_BIND_DISPATCH(JP_C8, 0x30);
		mDispatcher[0x01] = GB_BIND_DISPATCH(LD_R_I16, 0x01);	mDispatcher[0x11] = GB_BIND_DISPATCH(LD_R_I16, 0x11);		mDispatcher[0x21] = GB_BIND_DISPATCH(LD_R_I16, 0x21);		mDispatcher[0x31] = GB_BIND_DISPATCH(LD_R_I16, 0x31);
		mDispatcher[0x02] = GB_BIND_DISPATCH(LD_M_R, 0x02);		mDispatcher[0x12] = GB_BIND_DISPATCH(LD_M_R, 0x12);			mDispatcher[0x22] = GB_BIND_DISPATCH(LD_M_R, 0x22);			mDispatcher[0x32] = GB_BIND_DISPATCH(LD_M_R, 0x32);
		mDispatcher[0x03] = GB_BIND_DISPATCH(UN_R, 0x03);		mDispatcher[0x13] = GB_BIND_DISPATCH(UN_R, 0x13);			mDispatcher[0x23] = GB_BIND_DISPATCH(UN_R, 0x23);			mDispatcher[0x33] = GB_BIND_DISPATCH(UN_R, 0x33);
		mDispatcher[0x04] = GB_BIND_DISPATCH(UN_R, 0x04);		mDispatcher[0x14] = GB_BIND_DISPATCH(UN_R, 0x14);			mDispatcher[0x24] = GB_BIND_DISPATCH(UN_R, 0x24);			mDispatcher[0x34] = GB_BIND_DISPATCH(UN_R, 0x34);
		mDispatcher[0x05] = GB_BIND_DISPATCH(UN_R, 0x05);		mDispatcher[0x15] = GB_BIND_DISPATCH(UN_R, 0x15);			mDispatcher[0x25] = GB_BIND_DISPATCH(UN_R, 0x25);			mDispatcher[0x35] = GB_BIND_DISPATCH(UN_R, 0x35);
		mDispatcher[0x06] = GB_BIND_DISPATCH(LD_R_I8, 0x06);	mDispatcher[0x16] = GB_BIND_DISPATCH(LD_R_I8, 0x16);		mDispatcher[0x26] = GB_BIND_DISPATCH(LD_R_I8, 0x26);		mDispatcher[0x36] = GB_BIND_DISPATCH(LD_R_I8, 0x36);
		mDispatcher[0x07] = GB_BIND_DISPATCH(REG, 0x07);		mDispatcher[0x17] = GB_BIND_DISPATCH(REG, 0x17);			mDispatcher[0x27] = GB_BIND_DISPATCH(REG, 0x27);			mDispatcher[0x37] = GB_BIND_DISPATCH(REG, 0x37);
		mDispatcher[0x08] = GB_BIND_DISPATCH(LD_M_SP, 0x08);	mDispatcher[0x18] = GB_BIND_DISPATCH(JP_C8, 0x18);			mDispatcher[0x28] = GB_BIND_DISPATCH(JP_C8, 0x28);			mDispatcher[0x38] = GB_BIND_DISPATCH(JP_C8, 0x38);
		mDispatcher[0x09] = GB_BIND_DISPATCH(ADD_HL, 0x09);		mDispatcher[0x19] = GB_BIND_DISPATCH(ADD_HL, 0x19);			mDispatcher[0x29] = GB_BIND_DISPATCH(ADD_HL, 0x29);			mDispatcher[0x39] = GB_BIND_DISPATCH(ADD_HL, 0x39);
		mDispatcher[0x0A] = GB_BIND_DISPATCH(LD_R_M, 0x0A);		mDispatcher[0x1A] = GB_BIND_DISPATCH(LD_R_M, 0x1A);			mDispatcher[0x2A] = GB_BIND_DISPATCH(LD_R_M, 0x2A);			mDispatcher[0x3A] = GB_BIND_DISPATCH(LD_R_M, 0x3A);
		mDispatcher[0x0B] = GB_BIND_DISPATCH(UN_R, 0x0B);		mDispatcher[0x1B] = GB_BIND_DISPATCH(UN_R, 0x1B);			mDispatcher[0x2B] = GB_BIND_DISPATCH(UN_R, 0x2B);			mDispatcher[0x3B] = GB_BIND_DISPATCH(UN_R, 0x3B);
		mDispatcher[0x0C] = GB_BIND_DISPATCH(UN_R, 0x0C);		mDispatcher[0x1C] = GB_BIND_DISPATCH(UN_R, 0x1C);			mDispatcher[0x2C] = GB_BIND_DISPATCH(UN_R, 0x2C);			mDispatcher[0x3C] = GB_BIND_DISPATCH(UN_R, 0x3C);
		mDispatcher[0x0D] = GB_BIND_DISPATCH(UN_R, 0x0D);		mDispatcher[0x1D] = GB_BIND_DISPATCH(UN_R, 0x1D);			mDispatcher[0x2D] = GB_BIND_DISPATCH(UN_R, 0x2D);			mDispatcher[0x3D] = GB_BIND_DISPATCH(UN_R, 0x3D);
		mDispatcher[0x0E] = GB_BIND_DISPATCH(LD_R_I8, 0x0E);	mDispatcher[0x1E] = GB_BIND_DISPATCH(LD_R_I8, 0x1E);		mDispatcher[0x2E] = GB_BIND_DISPATCH(LD_R_I8, 0x2E);		mDispatcher[0x3E] = GB_BIND_DISPATCH(LD_R_I8, 0x3E);
		mDispatcher[0x0F] = GB_BIND_DISPATCH(REG, 0x0F);		mDispatcher[0x1F] = GB_BIND_DISPATCH(REG, 0x1F);			mDispatcher[0x2F] = GB_BIND_DISPATCH(REG, 0x2F);			mDispatcher[0x3F] = GB_BIND_DISPATCH(REG, 0x3F);
		mDispatcher[0x40] = GB_BIND_DISPATCH(NOP, 0x40);		mDispatcher[0x50] = GB_BIND_DISPATCH(LD_R_R, 0x50);			mDispatcher[0x60] = GB_BIND_DISPATCH(LD_R_R, 0x60);			mDispatcher[0x70] = GB_BIND_DISPATCH(LD_R_R, 0x70);
		mDispatcher[0x41] = GB_BIND_DISPATCH(LD_R_R, 0x41);		mDispatcher[0x51] = GB_BIND_DISPATCH(LD_R_R, 0x51);			mDispatcher[0x61] = GB_BIND_DISPATCH(LD_R_R, 0x61);			mDispatcher[0x71] = GB_BIND_DISPATCH(LD_R_R, 0x71);
		mDispatcher[0x42] = GB_BIND_DISPATCH(LD_R_R, 0x42);		mDispatcher[0x52] = GB_BIND_DISPATCH(NOP, 0x52);			mDispatcher[0x62] = GB_BIND_DISPATCH(LD_R_R, 0x62);			mDispatcher[0x72] = GB_BIND_DISPATCH(LD_R_R, 0x72);
		mDispatcher[0x43] = GB_BIND_DISPATCH(LD_R_R, 0x43);		mDispatcher[0x53] = GB_BIND_DISPATCH(LD_R_R, 0x53);			mDispatcher[0x63] = GB_BIND_DISPATCH(LD_R_R, 0x63);			mDispatcher[0x73] = GB_BIND_DISPATCH(LD_R_R, 0x73);
		mDispatcher[0x44] = GB_BIND_DISPATCH(LD_R_R, 0x44);		mDispatcher[0x54] = GB_BIND_DISPATCH(LD_R_R, 0x54);			mDispatcher[0x64] = GB_BIND_DISPATCH(NOP, 0x64);			mDispatcher[0x74] = GB_BIND_DISPATCH(LD_R_R, 0x74);
		mDispatcher[0x45] = GB_BIND_DISPATCH(LD_R_R, 0x45);		mDispatcher[0x55] = GB_BIND_DISPATCH(LD_R_R, 0x55);			mDispatcher[0x65] = GB_BIND_DISPATCH(LD_R_R, 0x65);			mDispatcher[0x75] = GB_BIND_DISPATCH(LD_R_R, 0x75);
		mDispatcher[0x46] = GB_BIND_DISPATCH(LD_R_R, 0x46);		mDispatcher[0x56] = GB_BIND_DISPATCH(LD_R_R, 0x56);			mDispatcher[0x66] = GB_BIND_DISPATCH(LD_R_R, 0x66);			mDispatcher[0x76] = GB_BIND_DISPATCH(HALT, 0x76);
		mDispatcher[0x47] = GB_BIND_DISPATCH(LD_R_R, 0x47);		mDispatcher[0x57] = GB_BIND_DISPATCH(LD_R_R, 0x57);			mDispatcher[0x67] = GB_BIND_DISPATCH(LD_R_R, 0x67);			mDispatcher[0x77] = GB_BIND_DISPATCH(LD_R_R, 0x77);
		mDispatcher[0x48] = GB_BIND_DISPATCH(LD_R_R, 0x48);		mDispatcher[0x58] = GB_BIND_DISPATCH(LD_R_R, 0x58);			mDispatcher[0x68] = GB_BIND_DISPATCH(LD_R_R, 0x68);			mDispatcher[0x78] = GB_BIND_DISPATCH(LD_R_R, 0x78);
		mDispatcher[0x49] = GB_BIND_DISPATCH(NOP, 0x49);		mDispatcher[0x59] = GB_BIND_DISPATCH(LD_R_R, 0x59);			mDispatcher[0x69] = GB_BIND_DISPATCH(LD_R_R, 0x69);			mDispatcher[0x79] = GB_BIND_DISPATCH(LD_R_R, 0x79);
		mDispatcher[0x4A] = GB_BIND_DISPATCH(LD_R_R, 0x4A);		mDispatcher[0x5A] = GB_BIND_DISPATCH(LD_R_R, 0x5A);			mDispatcher[0x6A] = GB_BIND_DISPATCH(LD_R_R, 0x6A);			mDispatcher[0x7A] = GB_BIND_DISPATCH(LD_R_R, 0x7A);
		mDispatcher[0x4B] = GB_BIND_DISPATCH(LD_R_R, 0x4B);		mDispatcher[0x5B] = GB_BIND_DISPATCH(NOP, 0x5B);			mDispatcher[0x6B] = GB_BIND_DISPATCH(LD_R_R, 0x6B);			mDispatcher[0x7B] = GB_BIND_DISPATCH(LD_R_R, 0x7B);
		mDispatcher[0x4C] = GB_BIND_DISPATCH(LD_R_R, 0x4C);		mDispatcher[0x5C] = GB_BIND_DISPATCH(LD_R_R, 0x5C);			mDispatcher[0x6C] = GB_BIND_DISPATCH(LD_R_R, 0x6C);			mDispatcher[0x7C] = GB_BIND_DISPATCH(LD_R_R, 0x7C);
		mDispatcher[0x4D] = GB_BIND_DISPATCH(LD_R_R, 0x4D);		mDispatcher[0x5D] = GB_BIND_DISPATCH(LD_R_R, 0x5D);			mDispatcher[0x6D] = GB_BIND_DISPATCH(NOP, 0x6D);			mDispatcher[0x7D] = GB_BIND_DISPATCH(LD_R_R, 0x7D);
		mDispatcher[0x4E] = GB_BIND_DISPATCH(LD_R_R, 0x4E);		mDispatcher[0x5E] = GB_BIND_DISPATCH(LD_R_R, 0x5E);			mDispatcher[0x6E] = GB_BIND_DISPATCH(LD_R_R, 0x6E);			mDispatcher[0x7E] = GB_BIND_DISPATCH(LD_R_R, 0x7E);
		mDispatcher[0x4F] = GB_BIND_DISPATCH(LD_R_R, 0x4F);		mDispatcher[0x5F] = GB_BIND_DISPATCH(LD_R_R, 0x5F);			mDispatcher[0x6F] = GB_BIND_DISPATCH(LD_R_R, 0x6F);			mDispatcher[0x7F] = GB_BIND_DISPATCH(NOP, 0x7F);
		mDispatcher[0x80] = GB_BIND_DISPATCH(ALU_T, 0x80);		mDispatcher[0x90] = GB_BIND_DISPATCH(ALU_T, 0x90);			mDispatcher[0xA0] = GB_BIND_DISPATCH(ALU_T, 0xA0);			mDispatcher[0xB0] = GB_BIND_DISPATCH(ALU_T, 0xB0);
		mDispatcher[0x81] = GB_BIND_DISPATCH(ALU_T, 0x81);		mDispatcher[0x91] = GB_BIND_DISPATCH(ALU_T, 0x91);			mDispatcher[0xA1] = GB_BIND_DISPATCH(ALU_T, 0xA1);			mDispatcher[0xB1] = GB_BIND_DISPATCH(ALU_T, 0xB1);
		mDispatcher[0x82] = GB_BIND_DISPATCH(ALU_T, 0x82);		mDispatcher[0x92] = GB_BIND_DISPATCH(ALU_T, 0x92);			mDispatcher[0xA2] = GB_BIND_DISPATCH(ALU_T, 0xA2);			mDispatcher[0xB2] = GB_BIND_DISPATCH(ALU_T, 0xB2);
		mDispatcher[0x83] = GB_BIND_DISPATCH(ALU_T, 0x83);		mDispatcher[0x93] = GB_BIND_DISPATCH(ALU_T, 0x93);			mDispatcher[0xA3] = GB_BIND_DISPATCH(ALU_T, 0xA3);			mDispatcher[0xB3] = GB_BIND_DISPATCH(ALU_T, 0xB3);
		mDispatcher[0x84] = GB_BIND_DISPATCH(ALU_T, 0x84);		mDispatcher[0x94] = GB_BIND_DISPATCH(ALU_T, 0x94);			mDispatcher[0xA4] = GB_BIND_DISPATCH(ALU_T, 0xA4);			mDispatcher[0xB4] = GB_BIND_DISPATCH(ALU_T, 0xB4);
		mDispatcher[0x85] = GB_BIND_DISPATCH(ALU_T, 0x85);		mDispatcher[0x95] = GB_BIND_DISPATCH(ALU_T, 0x95);			mDispatcher[0xA5] = GB_BIND_DISPATCH(ALU_T, 0xA5);			mDispatcher[0xB5] = GB_BIND_DISPATCH(ALU_T, 0xB5);
		mDispatcher[0x86] = GB_BIND_DISPATCH(ALU_T, 0x86);		mDispatcher[0x96] = GB_BIND_DISPATCH(ALU_T, 0x96);			mDispatcher[0xA6] = GB_BIND_DISPATCH(ALU_T, 0xA6);			mDispatcher[0xB6] = GB_BIND_DISPATCH(ALU_T, 0xB6);
		mDispatcher[0x87] = GB_BIND_DISPATCH(ALU_T, 0x87);		mDispatcher[0x97] = GB_BIND_DISPATCH(ALU_T, 0x97);			mDispatcher[0xA7] = GB_BIND_DISPATCH(ALU_T, 0xA7);			mDispatcher[0xB7] = GB_BIND_DISPATCH(ALU_T, 0xB7);
		mDispatcher[0x88] = GB_BIND_DISPATCH(ALU_T, 0x88);		mDispatcher[0x98] = GB_BIND_DISPATCH(ALU_T, 0x98);			mDispatcher[0xA8] = GB_BIND_DISPATCH(ALU_T, 0xA8);			mDispatcher[0xB8] = GB_BIND_DISPATCH(ALU_T, 0xB8);
		mDispatcher[0x89] = GB_BIND_DISPATCH(ALU_T, 0x89);		mDispatcher[0x99] = GB_BIND_DISPATCH(ALU_T, 0x99);			mDispatcher[0xA9] = GB_BIND_DISPATCH(ALU_T, 0xA9);			mDispatcher[0xB9] = GB_BIND_DISPATCH(ALU_T, 0xB9);
		mDispatcher[0x8A] = GB_BIND_DISPATCH(ALU_T, 0x8A);		mDispatcher[0x9A] = GB_BIND_DISPATCH(ALU_T, 0x9A);			mDispatcher[0xAA] = GB_BIND_DISPATCH(ALU_T, 0xAA);			mDispatcher[0xBA] = GB_BIND_DISPATCH(ALU_T, 0xBA);
		mDispatcher[0x8B] = GB_BIND_DISPATCH(ALU_T, 0x8B);		mDispatcher[0x9B] = GB_BIND_DISPATCH(ALU_T, 0x9B);			mDispatcher[0xAB] = GB_BIND_DISPATCH(ALU_T, 0xAB);			mDispatcher[0xBB] = GB_BIND_DISPATCH(ALU_T, 0xBB);
		mDispatcher[0x8C] = GB_BIND_DISPATCH(ALU_T, 0x8C);		mDispatcher[0x9C] = GB_BIND_DISPATCH(ALU_T, 0x9C);			mDispatcher[0xAC] = GB_BIND_DISPATCH(ALU_T, 0xAC);			mDispatcher[0xBC] = GB_BIND_DISPATCH(ALU_T, 0xBC);
		mDispatcher[0x8D] = GB_BIND_DISPATCH(ALU_T, 0x8D);		mDispatcher[0x9D] = GB_BIND_DISPATCH(ALU_T, 0x9D);			mDispatcher[0xAD] = GB_BIND_DISPATCH(ALU_T, 0xAD);			mDispatcher[0xBD] = GB_BIND_DISPATCH(ALU_T, 0xBD);
		mDispatcher[0x8E] = GB_BIND_DISPATCH(ALU_T, 0x8E);		mDispatcher[0x9E] = GB_BIND_DISPATCH(ALU_T, 0x9E);			mDispatcher[0xAE] = GB_BIND_DISPATCH(ALU_T, 0xAE);			mDispatcher[0xBE] = GB_BIND_DISPATCH(ALU_T, 0xBE);
		mDispatcher[0x8F] = GB_BIND_DISPATCH(ALU_T, 0x8F);		mDispatcher[0x9F] = GB_BIND_DISPATCH(ALU_T, 0x9F);			mDispatcher[0xAF] = GB_BIND_DISPATCH(ALU_T, 0xAF);			mDispatcher[0xBF] = GB_BIND_DISPATCH(ALU_T, 0xBF);
		mDispatcher[0xC0] = GB_BIND_DISPATCH(RET, 0xC0);		mDispatcher[0xD0] = GB_BIND_DISPATCH(RET, 0xD0);			mDispatcher[0xE0] = GB_BIND_DISPATCH(LD_R_STK, 0xE0);		mDispatcher[0xF0] = GB_BIND_DISPATCH(LD_R_STK, 0xF0);
		mDispatcher[0xC1] = GB_BIND_DISPATCH(POP, 0xC1);		mDispatcher[0xD1] = GB_BIND_DISPATCH(POP, 0xD1);			mDispatcher[0xE1] = GB_BIND_DISPATCH(POP, 0xE1);			mDispatcher[0xF1] = GB_BIND_DISPATCH(POP, 0xF1);
		mDispatcher[0xC2] = GB_BIND_DISPATCH(JP_C16, 0xC2);		mDispatcher[0xD2] = GB_BIND_DISPATCH(JP_C16, 0xD2);			mDispatcher[0xE2] = GB_BIND_DISPATCH(LD_R_STK, 0xE2);		mDispatcher[0xF2] = GB_BIND_DISPATCH(LD_R_STK, 0xF2);
		mDispatcher[0xC3] = GB_BIND_DISPATCH(JP_I16, 0xC3);		mDispatcher[0xD3] = GB_BIND_DISPATCH(ASSERT, 0xD3);			mDispatcher[0xE3] = GB_BIND_DISPATCH(ASSERT, 0xE3);			mDispatcher[0xF3] = GB_BIND_DISPATCH(DI, 0xF3);
		mDispatcher[0xC4] = GB_BIND_DISPATCH(CALL, 0xC4);		mDispatcher[0xD4] = GB_BIND_DISPATCH(CALL, 0xD4);			mDispatcher[0xE4] = GB_BIND_DISPATCH(ASSERT, 0xE4);			mDispatcher[0xF4] = GB_BIND_DISPATCH(ASSERT, 0xF4);
		mDispatcher[0xC5] = GB_BIND_DISPATCH(PUSH, 0xC5);		mDispatcher[0xD5] = GB_BIND_DISPATCH(PUSH, 0xD5);			mDispatcher[0xE5] = GB_BIND_DISPATCH(PUSH, 0xE5);			mDispatcher[0xF5] = GB_BIND_DISPATCH(PUSH, 0xF5);
		mDispatcher[0xC6] = GB_BIND_DISPATCH(ALU_T, 0xC6);		mDispatcher[0xD6] = GB_BIND_DISPATCH(ALU_T, 0xD6);			mDispatcher[0xE6] = GB_BIND_DISPATCH(ALU_T, 0xE6);			mDispatcher[0xF6] = GB_BIND_DISPATCH(ALU_T, 0xF6);
		mDispatcher[0xC7] = GB_BIND_DISPATCH(ASSERT, 0xC7);		mDispatcher[0xD7] = GB_BIND_DISPATCH(ASSERT, 0xD7);			mDispatcher[0xE7] = GB_BIND_DISPATCH(ASSERT, 0xE7);			mDispatcher[0xF7] = GB_BIND_DISPATCH(ASSERT, 0xF7);
		mDispatcher[0xC8] = GB_BIND_DISPATCH(RET, 0xC8);		mDispatcher[0xD8] = GB_BIND_DISPATCH(RET, 0xD8);			mDispatcher[0xE8] = GB_BIND_DISPATCH(ASSERT, 0xE8);			mDispatcher[0xF8] = GB_BIND_DISPATCH(ASSERT, 0xF8);
		mDispatcher[0xC9] = GB_BIND_DISPATCH(RET, 0xC9);		mDispatcher[0xD9] = GB_BIND_DISPATCH(RETI, 0xD9);			mDispatcher[0xE9] = GB_BIND_DISPATCH(LD_HL_PC, 0xE9);		mDispatcher[0xF9] = GB_BIND_DISPATCH(ASSERT, 0xF9);
		mDispatcher[0xCA] = GB_BIND_DISPATCH(JP_C16, 0xCA);		mDispatcher[0xDA] = GB_BIND_DISPATCH(JP_C16, 0xDA);			mDispatcher[0xEA] = GB_BIND_DISPATCH(LD_R_STK16, 0xEA);		mDispatcher[0xFA] = GB_BIND_DISPATCH(LD_R_STK16, 0xFA);
		mDispatcher[0xCB] = GB_BIND_DISPATCH(CB, 0xCB);			mDispatcher[0xDB] = GB_BIND_DISPATCH(ASSERT, 0xDB);			mDispatcher[0xEB] = GB_BIND_DISPATCH(ASSERT, 0xEB);			mDispatcher[0xFB] = GB_BIND_DISPATCH(EI, 0xFB);
		mDispatcher[0xCC] = GB_BIND_DISPATCH(CALL, 0xCC);		mDispatcher[0xDC] = GB_BIND_DISPATCH(CALL, 0xDC);			mDispatcher[0xEC] = GB_BIND_DISPATCH(ASSERT, 0xEC);			mDispatcher[0xFC] = GB_BIND_DISPATCH(ASSERT, 0xFC);
		mDispatcher[0xCD] = GB_BIND_DISPATCH(CALL, 0xCD);		mDispatcher[0xDD] = GB_BIND_DISPATCH(ASSERT, 0xDD);			mDispatcher[0xED] = GB_BIND_DISPATCH(ASSERT, 0xED);			mDispatcher[0xFD] = GB_BIND_DISPATCH(ASSERT, 0xFD);
		mDispatcher[0xCE] = GB_BIND_DISPATCH(ALU_T, 0xCE);		mDispatcher[0xDE] = GB_BIND_DISPATCH(ALU_T, 0xDE);			mDispatcher[0xEE] = GB_BIND_DISPATCH(ALU_T, 0xEE);			mDispatcher[0xFE] = GB_BIND_DISPATCH(ALU_T, 0xFE);
		mDispatcher[0xCF] = GB_BIND_DISPATCH(RST, 0xCF);		mDispatcher[0xDF] = GB_BIND_DISPATCH(RST, 0xDF);			mDispatcher[0xEF] = GB_BIND_DISPATCH(RST, 0xEF);			mDispatcher[0xFF] = GB_BIND_DISPATCH(RST, 0xFF);
	}
#pragma endregion

#pragma region OpCodeFunctions

	void CPU::CB(OpCode op)
	{
		mCBInstruction = true;

		OpCode instruction = AddressBus::Read(mRegisters[WordReg::PC]++);
		switch (instruction.x())
		{
		case 0: ROT_R(instruction); break;
		case 1: BIT_R(instruction); break;
		case 2: RES_R(instruction); break;
		case 3: SET_R(instruction); break;
		default: break;
		}
	}

	void CPU::LD_R_R(OpCode op)
	{
		Byte src;
		READ_REG(op.z(), src);
		WRITE_REG(op.y(), src);

		if (!mSession->print)
			return;
			
		EMU_TRACE("0x{0:04X}: LD {1}, {2}", mSession->lastPC, RegToString(op.z()), RegToString(op.y()));
	}

	void CPU::LD_M_R(OpCode op)
	{
		Byte p = op.p();
		SByte incDec = 0;
		if (p > 1)
			incDec = -1 * ((SByte)(p * 2) - 5);
		if (p == 3)
			p--;

		Byte src;
		READ_REG(7, src); // 7 targets A register

		Word& address = mRegisters[sRegisterPairs[p]];
		AddressBus::Write(address, src);
		address += incDec;

		if (!mSession->print)
			return;

		std::string sym;
		if (incDec)
		{
			if (incDec > 0)
				sym = "+";
			else
				sym = "-";
		}
		EMU_TRACE("0x{0:04X}: LD {1}{2}, A", mSession->lastPC, Emu::Enum::ToString(sRegisterPairs[p]), sym);
	}

	void CPU::LD_R_M(OpCode op)
	{
		Byte p = op.p();
		SByte incDec = 0;
		if (p > 1)
			incDec = -1 * ((SByte)(p * 2) - 5);
		if (p == 3)
			p--;

		Byte& regA = mRegisters[ByteReg::A];
		Word& address = mRegisters[sRegisterPairs[p]];

		regA = AddressBus::Read(address);
		address += incDec;

		if (!mSession->print)
			return;

		std::string sym;
		if (incDec)
		{
			if (incDec > 0)
				sym = "+";
			else
				sym = "-";
		}
		EMU_TRACE("0x{0:04X}: LD A, {1}{2}", mSession->lastPC, Emu::Enum::ToString(sRegisterPairs[p]), sym);
	}

	void CPU::LD_R_I8(OpCode op)
	{
		Byte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
		WRITE_REG(op.y(), imm);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: LD {1}, 0x{2:02X}", mSession->lastPC, RegToString(op.y()), imm);
	}

	void CPU::LD_R_I16(OpCode op)
	{
		Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word imm16 = lo | (hi << 8);

		WordReg target = sRegisterPairs[op.p()];
		mRegisters[target] = imm16;

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: LD {1}, 0x{2:04X}", mSession->lastPC, Emu::Enum::ToString(target), imm16);
	}

	void CPU::LD_R_STK(OpCode op)
	{
		Byte addr = (op.z() & GB_BIT(1)) ? mRegisters[ByteReg::C] : AddressBus::Read(mRegisters[WordReg::PC]++);
		bool immToReg = op & GB_BIT(4);

		if (immToReg)
			mRegisters[ByteReg::A] = AddressBus::Read(0xFF00 + (Word)addr);
		else
			AddressBus::Write(0xFF00 + (Word)addr, mRegisters[ByteReg::A]);

		if (!mSession->print)
			return;

		if (immToReg)
			EMU_TRACE("0x{0:04X}: LD A, 0xFF{1:02X}", mSession->lastPC, addr);
		else
			EMU_TRACE("0x{0:04X}: LD 0xFF{1:02X}, A", mSession->lastPC, addr);
	}

	void CPU::LD_R_STK16(OpCode op)
	{
		Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word imm = lo | (hi << 8);
		bool immToReg = op & GB_BIT(4);

		if (immToReg)
			mRegisters[ByteReg::A] = AddressBus::Read(imm);
		else
			AddressBus::Write(imm, mRegisters[ByteReg::A]);

		if (!mSession->print)
			return;

		if (immToReg)
			EMU_TRACE("0x{0:04X}: LD A, 0x{1:04X}", mSession->lastPC, imm);
		else
			EMU_TRACE("0x{0:04X}: LD 0x{1:04X}, A", mSession->lastPC, imm);

	}

	void CPU::LD_M_SP(OpCode op)
	{
		Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word address = lo | (hi << 8);

		Word sp = mRegisters[WordReg::SP];
		AddressBus::Write(address++, sp & 0x0F);
		AddressBus::Write(address, (sp & 0xF0) >> 4);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: LD 0x{1:04X}, SP", mSession->lastPC, address);
	}

	void CPU::ROT_R(OpCode op)
	{
		Byte target = op.z();
		switch (op.y())
		{
		case 0: RLC_R(target); break;
		case 1: RRC_R(target); break;
		case 2: RL_R(target); break;
		case 3: RR_R(target); break;
		case 4: SLA_R(target); break;
		case 5: SRA_R(target); break;
		case 6: SWAP_R(target); break;
		case 7: SRL_R(target); break;
		}
	}

	void CPU::JP_C8(OpCode op)
	{
		SByte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
		Byte condTarget = op.y();

		bool hasCondition = (condTarget & 0x04);
		if (hasCondition) 
		{
			if (CheckCondition((Condition)(condTarget - 0x04)))
				mRegisters[WordReg::PC] += imm;
		}
		else
			mRegisters[WordReg::PC] += imm;

		if (!mSession->print)
			return;

		std::string condition = std::string(Emu::Enum::ToString((Condition)(condTarget - 0x04)));

		EMU_TRACE("0x{0:04X}: JP {1}, 0x{2:02X}", mSession->lastPC, condition, imm);
	}

	void CPU::JP_C16(OpCode op)
	{
		Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word imm = lo | (hi << 8);

		Byte condTarget = op.y();
		if (CheckCondition((Condition)(condTarget)))
			mRegisters[WordReg::PC] = imm;

		if (!mSession->print)
			return;

		std::string condition = std::string(Emu::Enum::ToString((Condition)condTarget));

		EMU_TRACE("0x{0:04X}: JP {1}, 0x{2:04X}", mSession->lastPC, condition, imm);
	}

	void CPU::JP_I16(OpCode op)
	{
		Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word imm = lo | (hi << 8);
		mRegisters[WordReg::PC] = imm;

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: JP 0x{1:04X}", mSession->lastPC, imm);
	}

	void CPU::UN_R(OpCode op)
	{
		switch (op.z())
		{
		case 3:
		{
			switch (op.q())
			{
			case 0: INC16(op.p()); break;
			case 1: DEC16(op.p()); break;
			}
			break;
		}
		case 4:
			INC(op.y());
			break;

		case 5:
			DEC(op.y());
			break;
		}
	}

	void CPU::CALL(OpCode op)
	{
		Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
		Word imm = lo | (hi << 8);

		// Condition not met
		if (!(op & 0x01) && !CheckCondition((Condition)op.y()))
			return;

		Word& pc = mRegisters[WordReg::PC];
		Word& sp = mRegisters[WordReg::SP];
		AddressBus::Write(--sp, (Byte)(pc >> 8));
		AddressBus::Write(--sp, (Byte)(pc & 0x00FF));
		pc = imm;

		if (!mSession->print)
			return;

		std::string condition;
		if (op & 0x01)
			condition = std::string(Emu::Enum::ToString((Condition)op.y())) + ", ";

		EMU_TRACE("0x{0:04X}: CALL {1}0x{2:04X}", mSession->lastPC, condition, imm);
	}

	void CPU::RET(OpCode op)
	{
		// Condition not met
		if (!(op & 0x01) && !CheckCondition((Condition)op.y()))
			return;

		Word& sp = mRegisters[WordReg::SP];
		Word& pc  = mRegisters[WordReg::PC];

		pc = (Word)AddressBus::Read(sp++);
		pc |= (Word)AddressBus::Read(sp++) << 8;

		if (!mSession->print)
			return;

		std::string condition;
		if (op & 0x01)
			condition = std::string(Emu::Enum::ToString((Condition)op.y()));

		EMU_TRACE("0x{0:04X}: RET {1}", mSession->lastPC, condition);
	}

	void CPU::RETI(OpCode op)
	{
		RET(op); 
		mInterruptsEnabled = true;

		if (!mSession->print)
			return;
		EMU_TRACE("0x{0:04X}: RETI", mSession->lastPC);
	}

	void CPU::RST(OpCode op)
	{
		Word& pc = mRegisters[WordReg::PC];
		Word& sp = mRegisters[WordReg::SP];
		AddressBus::Write(--sp, (Byte)(pc >> 8));
		AddressBus::Write(--sp, (Byte)(pc & 0x00FF));
		pc = op.y() * 0x08;

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RST {1}", mSession->lastPC, op.y());
	}

	void CPU::REG(OpCode op)
	{
		switch (op.y())
		{
		case 0: RLCA(); break;
		case 1: RRCA(); break;
		case 2: RLA(); break;
		case 3: RRA(); break;
		case 4: DAA(); break;
		case 5: CPL(); break;
		case 6: SCF(); break;
		case 7: CCF(); break;
		}
	}

	void CPU::EI(OpCode op)
	{
		mInterruptsEnabled = true;

		if (!mSession->print)
			return;
		EMU_TRACE("0x{0:04X}: EI", mSession->lastPC);
	}

	void CPU::DI(OpCode op)
	{
		mInterruptsEnabled = false;

		if (!mSession->print)
			return;
		EMU_TRACE("0x{0:04X}: DI", mSession->lastPC);
	}

	void CPU::HALT(OpCode op)
	{
		mHalted = true;

		if (!mSession->print)
			return;
		EMU_TRACE("0x{0:04X}: HALT", mSession->lastPC);
	}

	void CPU::STOP(OpCode op)
	{
		mRegisters[WordReg::PC]++;

		if (!mSession->print)
			return;
		EMU_TRACE("0x{0:04X}: STOP", mSession->lastPC);
	}



	// Instruction Implementations
		
	void CPU::NOP(OpCode opcode)
	{ 
		if (mSession->print) 
			EMU_TRACE("0x{0:04X}: NOP", mSession->lastPC);
	}

	void CPU::ALU_T(OpCode op)
	{
		switch (op.y())
		{
		case 0: ADD_R(op); break;
		case 1: ADC_R(op); break;
		case 2: SUB_R(op); break;
		case 3: SBC_R(op); break;
		case 4: AND_R(op); break;
		case 5: XOR_R(op); break;
		case 6: OR_R(op); break;
		case 7: CP_R(op); break;
		}
	}

	void CPU::ADD_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Word result = regA + value;
		regA = (Byte)result;

		BitField flags = 0;
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? GB_BIT(H_CARRY_BIT) : 0;
		flags |= ((result & 0x100) != 0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: ADD A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: ADD A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::ADC_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Word result = regA + value + (mFRegister.carry() ? 1 : 0);
		regA = (Byte)result;

		BitField flags = 0;
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? GB_BIT(H_CARRY_BIT) : 0;
		flags |= ((result & 0x100) != 0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: ADC A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: ADC A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::SUB_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Byte result = regA - value;
		regA = result;

		BitField flags = GB_BIT(SUBTRACT_BIT);
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? GB_BIT(H_CARRY_BIT) : 0;
		flags |= ((result & 0x100) != 0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: SUB A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: SUB A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::SBC_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Byte carry = mFRegister.carry() ? 1 : 0;
		SWord result = regA - value - carry;
		regA = (Byte)result;

		BitField flags = GB_BIT(SUBTRACT_BIT);
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= ((regACpy & 0xf) - (value & 0xf) - carry) ? GB_BIT(H_CARRY_BIT) : 0;
		flags |= (result < 0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: SBC A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: SBC A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::AND_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];

		regA &= value;

		BitField flags = GB_BIT(H_CARRY_BIT);
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;
		
		if (imm)
			EMU_TRACE("0x{0:04X}: AND A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: AND A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::XOR_R(OpCode op)
	{
		Byte value;
		bool imm = (op.x() == 3);
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];

		regA ^= value;

		BitField flags = 0;
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: XOR A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: XOR A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::OR_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte& regA = mRegisters[ByteReg::A];

		regA |= value;

		BitField flags = 0;
		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: OR A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: OR A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::CP_R(OpCode op)
	{
		Byte value;
		bool imm = op.x() == 3;
		if (imm)
			value = AddressBus::Read(mRegisters[WordReg::PC]++);
		else
			READ_REG(op.z(), value);

		Byte regA = mRegisters[ByteReg::A];

		Byte result = regA - value;

		BitField flags = GB_BIT(SUBTRACT_BIT);
		flags |= (result == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= ((regA & 0xf) - (value & 0xf) < 0) ? GB_BIT(H_CARRY_BIT) : 0;
		flags |= (regA < value) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		if (imm)
			EMU_TRACE("0x{0:04X}: CP A, 0x{1:02X}", mSession->lastPC, value);
		else
			EMU_TRACE("0x{0:04X}: CP A, {1}", mSession->lastPC, RegToString(op.z()));
	}

	void CPU::ADD_HL(OpCode op)
	{
		Word value = mRegisters[sRegisterPairs[op.p()]];

		Word& regHL = mRegisters[WordReg::HL];
		Word regHLCpy = regHL;

		Word result = regHL + value;
		regHL = (Byte)result;

		BitField flags = mFRegister.getFlags() & GB_BIT(ZERO_BIT);
		flags |= ((regHLCpy & 0xfff) + (value & 0xfff) > 0xfff) ? GB_BIT(H_CARRY_BIT) : 0;
		flags |= ((result & 0x10000) != 0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: ADD HL {1}", mSession->lastPC, Emu::Enum::ToString(sRegisterPairs[op.p()]));
	}

	void CPU::PUSH(OpCode op)
	{
		Word& sp = mRegisters[WordReg::SP];
		Word regPair = mRegisters[sRegisterPairs2[op.p()]];

		AddressBus::Write(--sp, (Byte)(regPair >> 8));
		AddressBus::Write(--sp, (Byte)(regPair & 0x00FF));

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: PUSH 0x{1:04X}", mSession->lastPC, regPair);
	}

	void CPU::POP(OpCode op)
	{
		Word& sp = mRegisters[WordReg::SP];
		Word& regPair = mRegisters[sRegisterPairs2[op.p()]];

		regPair = (Word)AddressBus::Read(sp++);
		regPair |= (Word)AddressBus::Read(sp++) << 8;

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: POP 0x{1:04X}", mSession->lastPC, regPair);
	}

	void CPU::INC(Byte target)
	{
		Byte reg;
		READ_REG(target, reg);
		reg++;
		WRITE_REG(target, reg);

		BitField flags = mFRegister.getFlags() & GB_BIT(CARRY_BIT);
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= ((reg & 0x0F) == 0x00) ? GB_BIT(H_CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: INC {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::DEC(Byte target)
	{
		Byte reg;
		READ_REG(target, reg);
		reg--;
		WRITE_REG(target, reg);

		BitField flags = mFRegister.getFlags() & GB_BIT(CARRY_BIT);
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= GB_BIT(SUBTRACT_BIT);
		flags |= ((reg & 0x0F) == 0x0F) ? GB_BIT(H_CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: DEC {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::INC16(Byte target)
	{
		Word& reg = mRegisters[sRegisterPairs[target]];
		reg++;

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: INC16 {1}", mSession->lastPC, Emu::Enum::ToString(sRegisterPairs[target]));
	}

	void CPU::DEC16(Byte target)
	{
		Word& reg = mRegisters[sRegisterPairs[target]];
		reg--;

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: DEC16 {1}", mSession->lastPC, Emu::Enum::ToString(sRegisterPairs[target]));
	}

	void CPU::RLCA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte finalBit = (regA & GB_BIT(7)) ? GB_BIT(0) : 0;
		regA = (Byte)(regA << 1) | finalBit;

		BitField flags = 0;
		flags |= finalBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RLCA", mSession->lastPC);
	}

	void CPU::RRCA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte firstBit = (regA & GB_BIT(0)) ? GB_BIT(7) : 0;
		regA = (Byte)(regA >> 1) | firstBit;

		BitField flags = 0;
		flags |= firstBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RRCA", mSession->lastPC);
	}

	void CPU::RLA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte firstBit = mFRegister.carry() ? GB_BIT(0) : 0;
		regA = (Byte)(regA << 1) | firstBit;

		BitField regAField = regA;

		BitField flags = 0;
		flags |= regAField.bit(7) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RLA", mSession->lastPC);
	}

	void CPU::RRA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte finalBit = mFRegister.carry() ? GB_BIT(7) : 0;
		regA = (Byte)(regA >> 1) | finalBit;

		BitField regAField = regA;

		BitField flags = 0;
		flags |= regAField.bit(0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RRA", mSession->lastPC);
	}

	void CPU::DAA()
	{
		Byte& regA = mRegisters[ByteReg::A];

		Word correction = mFRegister.carry() ? 0x60 : 0x00;

		if (mFRegister.hcarry() || (!mFRegister.subtr() && ((regA & 0x0F) > 9)))
			correction |= 0x06;

		if (mFRegister.carry() || (!mFRegister.subtr() && (regA > 0x99)))
			correction |= 0x60;

		if (mFRegister.subtr())
			regA = (Byte)(regA - correction);
		else 
			regA = (Byte)(regA + correction);

		BitField flags = mFRegister.getFlags() & GB_BIT(SUBTRACT_BIT);
		if (((correction << 2) & 0x100) != 0)
			flags |= GB_BIT(CARRY_BIT);

		flags |= (regA == 0) ? GB_BIT(ZERO_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: DAA", mSession->lastPC);
	}

	void CPU::CPL()
	{
		Byte& regA = mRegisters[ByteReg::A];
		regA = ~regA;

		BitField flags = mFRegister.getFlags();
		flags |= GB_BIT(SUBTRACT_BIT);
		flags |= GB_BIT(H_CARRY_BIT);
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: CPL", mSession->lastPC);
	}

	void CPU::SCF()
	{
		BitField flags = mFRegister.getFlags() & GB_BIT(ZERO_BIT);
		flags |= GB_BIT(CARRY_BIT);
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: SCF", mSession->lastPC);
	}

	void CPU::CCF()
	{
		BitField flags = mFRegister.getFlags() & GB_BIT(ZERO_BIT);
		flags |= !mFRegister.carry() ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: CCF", mSession->lastPC);
	}

	// CB Instructions Implementations

	void CPU::RLC_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		Byte finalBit = reg.bit(7) ? GB_BIT(0) : 0;
		reg = (Byte)(reg << 1) | finalBit;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= reg == 0 ? GB_BIT(ZERO_BIT) : 0;
		flags |= finalBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RLC {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::RRC_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		Byte firstBit = reg.bit(0) ? GB_BIT(7) : 0;
		reg = (Byte)(reg >> 1) | firstBit;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= reg == 0 ? GB_BIT(ZERO_BIT) : 0;
		flags |= firstBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RRC {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::RL_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		Byte firstBit = mFRegister.carry() ? GB_BIT(0) : 0;
		reg = (Byte)(reg << 1) | firstBit;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= reg.bit(7) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RL {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::RR_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		Byte finalBit = mFRegister.carry() ? GB_BIT(7) : 0;
		reg = (Byte)(reg >> 1) | finalBit;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= reg.bit(0) ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RR {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::SLA_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		bool carryBit = reg.bit(7);
		reg = reg << 1;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= carryBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: SLA {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::SRA_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		bool carryBit = reg.bit(0);
		Byte finalBit = reg & GB_BIT(7);
		reg = (reg >> 1) | finalBit;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= carryBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: SRA {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::SWAP_R(Byte target)
	{
		Byte reg;
		READ_REG(target, reg);
		Byte lo = (reg & 0x0F) << 4;
		Byte hi = (reg & 0xF0) >> 4;
		reg = hi | lo;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: SWAP {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::SRL_R(Byte target)
	{
		BitField reg;
		READ_REG(target, reg);
		bool firstBit = reg.bit(0);
		reg = reg >> 1;
		WRITE_REG(target, reg);

		BitField flags = 0;
		flags |= (reg == 0) ? GB_BIT(ZERO_BIT) : 0;
		flags |= firstBit ? GB_BIT(CARRY_BIT) : 0;
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: SRL {1}", mSession->lastPC, RegToString(target));
	}

	void CPU::BIT_R(OpCode op)
	{
		BitField value;
		Byte target = op.z();
		Byte bit = op.y();

		READ_REG(target, value);
		BitField flags = mFRegister.getFlags() & GB_BIT(CARRY_BIT);
		flags |= GB_BIT(H_CARRY_BIT);
		if (!value.bit(op.y()))
			flags |= GB_BIT(ZERO_BIT);
		mFRegister.setFlags(flags);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: BIT {1}, {2}", mSession->lastPC, bit, RegToString(target));
	}

	void CPU::RES_R(OpCode op)
	{
		Byte target = op.z();
		Byte bit = GB_BIT(op.y());
		Byte value;
		READ_REG(target, value);
		value &= ~bit;
		WRITE_REG(target, value);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: RES {1}, {2}", mSession->lastPC, bit, RegToString(target));
	}

	void CPU::SET_R(OpCode op)
	{
		Byte target = op.z();
		Byte bit = GB_BIT(op.y());
		Byte value;
		READ_REG(target, value);
		value |= bit;
		WRITE_REG(target, value);

		if (!mSession->print)
			return;

		EMU_TRACE("0x{0:04X}: SET {1}, {2}", mSession->lastPC, bit, RegToString(target));
	}

#pragma endregion
}

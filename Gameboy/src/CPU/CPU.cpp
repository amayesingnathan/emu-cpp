module Gameboy.CPU;

namespace GB {

	Byte CPU::tick()
	{
		WordRef pc = mRegisters[WordReg::PC];

		Word check = 0x0100;
		if (pc == check)
			int a = 4;

		Byte nextOp = AddressBus::Read(pc++);
		Byte cycles = mDispatcher[nextOp]();

		UpdateTimers(cycles);

		return cycles;
	}

	void CPU::handleInterrupts()
	{
		if (!mInterruptsEnabled)
			return;

		ByteBits req = AddressBus::Read(Addr::IF);
		ByteBits enabled = AddressBus::Read(Addr::IE);

		if (req == 0)
			return;

		for (Byte i = 0; i < 5; i++)
		{
			if (!req.test(i))
				continue;

			if (!enabled.test(i))
				continue;

			ServiceInterrupt((Interrupt)i);
		}
	}

	Byte CPU::GetOpCycles(Byte opcode)
	{
		if (mCBInstruction)
			return CYCLES[0xCB] + CB_CYCLES[opcode];

		if (mBranchTaken)
			return BRANCHED_CYCLES[opcode];

		return CYCLES[opcode];
	}

	void CPU::UpdateTimers(Byte cycles)
	{
		mDividerCounter.step(cycles);

		if (!mTimerCounter.step(cycles))
			return;

		mTimerCounter = AddressBus::Read(Addr::TMA);
		AddressBus::RequestInterrupt(Interrupt::Timer);
	}

	void CPU::ServiceInterrupt(Interrupt i)
	{
		mInterruptsEnabled = false;
		Byte& interruptFlag = AddressBus::Read(Addr::IF);

		ByteBits ifBits = interruptFlag;
		ifBits.reset((Byte)i);
		interruptFlag = (Byte)ifBits.to_ulong();

		WordRef pc = mRegisters[WordReg::PC];
		WordRef sp = mRegisters[WordReg::SP];
		AddressBus::Write(--sp, (Byte)(pc >> 8));
		AddressBus::Write(--sp, (Byte)(pc & 0x00FF));

		switch (i)
		{
		case Interrupt::V_Blank:
			pc = 0x40;
			break;

		case Interrupt::LCD_Stat:
			pc = 0x48;
			break;

		case Interrupt::Timer:
			pc = 0x50;
			break;

		case Interrupt::Serial:
			pc = 0x58;
			break;

		case Interrupt::Joypad:
			pc = 0x60;
			break;

		default:
			Emu::Assert(false);
			break;
		}
	}
}
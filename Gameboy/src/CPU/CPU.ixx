export module Gameboy.CPU;

import <array>;

export import Gameboy.Core.Types;
export import Gameboy.Memory.AddressBus;
export import Gameboy.Timers;
import Gameboy.CPU.OpCode;
import Gameboy.CPU.Register;
import Gameboy.CPU.Cycles;
import Gameboy.CPU.Interrupts;

export namespace GB {

	template<typename Op, Byte T>
	concept IsOpCode = std::is_same_v<Op, OpCode<T>>;

	class CPU
	{
	public:
		CPU() :
			mFRegister(mRegisters[ByteReg::F]),
			mDividerCounter(AddressBus::Read(Addr::DIV), FREQ_4096),
			mTimerCounter(AddressBus::Read(Addr::TIMA), FREQ_16384) {}

		Byte tick();
		Byte tickDebug(Word breakpoint);
		Byte tickOnce();

		void handleInterrupts();

		void startTimer() { mTimerCounter.start(); }
		void stopTimer() { mTimerCounter.stop(); }
		void setTimerFreq(Byte freq) { mTimerCounter.setFreq(freq); }

	private:
		Byte GetOpCycles(Byte opcode);
		void UpdateTimers(Byte cycles);

		void ServiceInterrupt(Interrupt i);

		template<Condition Cond>
		bool CheckCondition()
		{
			if constexpr (Cond == Condition::C)
				return mFRegister.carry();
			else if constexpr (Cond == Condition::NC)
				return !mFRegister.carry();
			else if constexpr (Cond == Condition::Z)
				return mFRegister.zero();
			else if constexpr (Cond == Condition::NZ)
				return !mFRegister.zero();
			else
				Emu::SAssert<false>();

			return false;
		}

	private:
		Registers mRegisters;
		FlagRegister mFRegister;

		Timer mDividerCounter;
		Timer mTimerCounter;

		bool mCBInstruction = false;
		bool mBranchTaken = false;
		bool mInterruptsEnabled = false;
		bool mHalted = false;

		friend class Gameboy;

#pragma region OpCode Implementations

	private:
		template<Byte Target> requires (Target >= 0 && Target < 8)
		Byte ReadByte()
		{
			if constexpr (Target == 0)
				return mRegisters[ByteReg::B];
			else if constexpr (Target == 1)
				return mRegisters[ByteReg::C];
			else if constexpr (Target == 2)
				return mRegisters[ByteReg::D];
			else if constexpr (Target == 3)
				return mRegisters[ByteReg::E];
			else if constexpr (Target == 4)
				return mRegisters[ByteReg::H];
			else if constexpr (Target == 5)
				return mRegisters[ByteReg::L];
			else if constexpr (Target == 6)
				return AddressBus::Read(mRegisters[WordReg::HL]);
			else if constexpr (Target == 7)
				return mRegisters[ByteReg::A];

			// Unreachable
			return 0;
		}

		template<Byte Target> requires (Target >= 0 && Target < 8)
		void WriteByte(Byte val)
		{
			if constexpr (Target == 0)
				mRegisters[ByteReg::B] = val;
			else if constexpr (Target == 1)
				mRegisters[ByteReg::C] = val;
			else if constexpr (Target == 2)
				mRegisters[ByteReg::D] = val;
			else if constexpr (Target == 3)
				mRegisters[ByteReg::E] = val;
			else if constexpr (Target == 4)
				mRegisters[ByteReg::H] = val;
			else if constexpr (Target == 5)
				mRegisters[ByteReg::L] = val;
			else if constexpr (Target == 6)
				AddressBus::Write(mRegisters[WordReg::HL], val);
			else if constexpr (Target == 7)
				mRegisters[ByteReg::A] = val;
		}

		template<Byte Target> requires (Target >= 0 && Target < 4)
		WordRef ReadWord()
		{
			if constexpr (Target == 0)
				return mRegisters[WordReg::BC];
			else if constexpr (Target == 1)
				return mRegisters[WordReg::DE];
			else if constexpr (Target == 2)
				return mRegisters[WordReg::HL];
			else if constexpr (Target == 3)
				return mRegisters[WordReg::SP];

			// Unreachable
			return mRegisters[WordReg::BC];
		}

		template<Byte Target> requires (Target >= 0 && Target < 4)
		void WriteWord(Word val)
		{
			if constexpr (Target == 0)
				mRegisters[WordReg::BC] = val;
			else if constexpr (Target == 1)
				mRegisters[WordReg::DE] = val;
			else if constexpr (Target == 2)
				mRegisters[WordReg::HL] = val;
			else if constexpr (Target == 3)
				mRegisters[WordReg::SP] = val;
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_R_R()
		{
			WriteByte<Op::Y>(ReadByte<Op::Z>());

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_M_R()
		{
			SByte incDec = 0;
			if constexpr (Op::P > 1)
				incDec = -1 * ((SByte)(Op::P * 2) - 5);

			constexpr Byte Target = []() -> Byte
			{
				if constexpr (Op::P == 3)
					return (Byte)2;
				return Op::P;
			}();
			;
			WordRef address = ReadWord<Target>();
			AddressBus::Write(address, ReadByte<7>()); // 7 targets A register
			address += incDec;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_R_M()
		{
			SByte incDec = 0;
			if constexpr (Op::P > 1)
				incDec = -1 * ((SByte)(Op::P * 2) - 5);

			constexpr Byte Target = []() -> Byte
			{
				if constexpr (Op::P == 3)
					return (Byte)2;
				return Op::P;
			}();

			WordRef address = ReadWord<Target>();
			mRegisters[ByteReg::A] = AddressBus::Read(address);
			address += incDec;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_R_I8()
		{
			Byte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
			WriteByte<Op::Y>(imm);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_R_I16()
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word imm16 = lo | (hi << 8);
			WriteWord<Op::P>(imm16);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_R_STK()
		{
			Byte addr = [this]() -> Byte
			{
				if constexpr (Op::Test(1))
					return mRegisters[ByteReg::C];
				return AddressBus::Read(mRegisters[WordReg::PC]++);
			}();

			if constexpr (Op::Test(4))
				mRegisters[ByteReg::A] = AddressBus::Read(0xFF00 + (Word)addr);
			else
				AddressBus::Write(0xFF00 + (Word)addr, mRegisters[ByteReg::A]);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_R_STK16()
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word imm = lo | (hi << 8);

			if constexpr (Op::Test(4))
				mRegisters[ByteReg::A] = AddressBus::Read(imm);
			else
				AddressBus::Write(imm, mRegisters[ByteReg::A]);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_HL_PC()
		{
			mRegisters[WordReg::PC] = mRegisters[WordReg::HL];

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_HL_SP()
		{
			SByte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word sp = mRegisters[WordReg::SP];

			USize result = sp + imm;
			mRegisters[WordReg::HL] = (Word)result;

			mFRegister.setZero(false);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(((sp & 0xF) + (imm & 0xF)) & 0x10);
			mFRegister.setCarry(result & 0x100);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_HL_SP16()
		{
			mRegisters[WordReg::PC] = mRegisters[WordReg::SP];

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte LD_M_SP()
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word address = lo | (hi << 8);

			Word sp = mRegisters[WordReg::SP];
			AddressBus::Write(address, sp & 0x0F);
			AddressBus::Write(address + 1, (sp & 0xF0) >> 4);

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte ROT_R()
		{
			if constexpr (Op::Y == 0)
				return RLC_R<Op, T>();
			else if constexpr (Op::Y == 1)
				return RRC_R<Op, T>();
			else if constexpr (Op::Y == 2)
				return RL_R<Op, T>();
			else if constexpr (Op::Y == 3)
				return RR_R<Op, T>();
			else if constexpr (Op::Y == 4)
				return SLA_R<Op, T>();
			else if constexpr (Op::Y == 5)
				return SRA_R<Op, T>();
			else if constexpr (Op::Y == 6)
				return SWAP_R<Op, T>();
			else if constexpr (Op::Y == 7)
				return SRL_R<Op, T>();
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte BIT_R()
		{
			ByteBits regBits(ReadByte<Op::Z>());

			mFRegister.setZero(!regBits.test(Op::Y));
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(true);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RES_R()
		{
			ByteBits regBits(ReadByte<Op::Z>());
			regBits.reset(Op::Y);
			WriteByte<Op::Z>((Byte)regBits.to_ulong());

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SET_R()
		{
			ByteBits regBits(ReadByte<Op::Z>());
			regBits.set(Op::Y);
			WriteByte<Op::Z>((Byte)regBits.to_ulong());

			return CB_CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte ADD_HL()
		{
			Word value = ReadWord<Op::P>();
			WordRef regHL = mRegisters[WordReg::HL];

			USize result = regHL + value;

			mFRegister.setSubtr(false);
			mFRegister.setHCarry(((regHL & 0xFFF) + (value & 0xFFF) & 0x1000));
			mFRegister.setCarry(result & 0x10000);

			regHL = (Word)result;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte JP_C8()
		{
			SByte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
			if constexpr (Op::Test(5))
			{
				constexpr Condition Cond = Op::AsCondition(Op::Y - 4);
				mBranchTaken = CheckCondition<Cond>();
				if (!mBranchTaken)
					return CYCLES[Op::Val];
			}

			mRegisters[WordReg::PC] += imm;

			return BRANCHED_CYCLES[Op::Val];

		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte JP_C16()
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word imm = lo | (hi << 8);

			constexpr Condition Cond = Op::AsCondition(Op::Y);
			if (mBranchTaken = CheckCondition<Cond>())
				mRegisters[WordReg::PC] = imm;

			return mBranchTaken ? BRANCHED_CYCLES[Op::Val] : CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte JP_I16()
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word imm = lo | (hi << 8);
			mRegisters[WordReg::PC] = imm;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte UN_R()
		{
			if constexpr (Op::Z == 3)
			{
				if constexpr (Op::Q == 0)
					return INC16<Op, T>();
				else if constexpr (Op::Q == 1)
					return DEC16<Op, T>();
			}
			else if constexpr (Op::Z == 4)
				return INC<Op, T>();
			else if constexpr (Op::Z == 5)
				return DEC<Op, T>();
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte CALL()
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);

			// Using condition?
			if constexpr (!Op::Test(0))
			{
				// Branch taken?
				constexpr Condition Cond = Op::AsCondition(Op::Y);
				mBranchTaken = CheckCondition<Cond>();
				if (!mBranchTaken)
					return CYCLES[Op::Val];
			}

			Word imm = lo | (hi << 8);
			WordRef pc = mRegisters[WordReg::PC];
			WordRef sp = mRegisters[WordReg::SP];
			AddressBus::Write(--sp, (Byte)(pc >> 8));
			AddressBus::Write(--sp, (Byte)(pc & 0x00FF));
			pc = imm;

			return BRANCHED_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RET()
		{
			// Using condition?
			if constexpr (!Op::Test(0))
			{
				// Branch taken?
				constexpr Condition Cond = Op::AsCondition(Op::Y);
				mBranchTaken = CheckCondition<Cond>();
				if (!mBranchTaken)
					return CYCLES[Op::Val];
			}

			WordRef sp = mRegisters[WordReg::SP];
			WordRef pc = mRegisters[WordReg::PC];

			Word newPC = (Word)AddressBus::Read(sp++);
			newPC |= (Word)AddressBus::Read(sp++) << 8;
			pc = newPC;

			return BRANCHED_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RETI()
		{
			RET<Op, T>();
			mInterruptsEnabled = true;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte PUSH()
		{
			WordRef sp = mRegisters[WordReg::SP];
			Word regPair = ReadWord<Op::P>();

			AddressBus::Write(--sp, (Byte)(regPair >> 8));
			AddressBus::Write(--sp, (Byte)(regPair & 0x00FF));

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte POP()
		{
			WordRef sp = mRegisters[WordReg::SP];
			WordRef regPair = ReadWord<Op::P>();

			Word popAddr = (Word)AddressBus::Read(sp++) | (Word)AddressBus::Read(sp++) << 8;

			regPair = popAddr;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RST()
		{
			constexpr Word InterruptLocation = Op::Y * 0x08;

			WordRef pc = mRegisters[WordReg::PC];
			WordRef sp = mRegisters[WordReg::SP];
			AddressBus::Write(--sp, (Byte)(pc >> 8));
			AddressBus::Write(--sp, (Byte)(pc & 0x00FF));
			pc = InterruptLocation;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte REG()
		{
			if constexpr (Op::Y == 0)
				return RLCA();
			else if constexpr (Op::Y == 1)
				return RRCA();
			else if constexpr (Op::Y == 2)
				return RLA();
			else if constexpr (Op::Y == 3)
				return RRA();
			else if constexpr (Op::Y == 4)
				return DAA();
			else if constexpr (Op::Y == 5)
				return CPL();
			else if constexpr (Op::Y == 6)
				return SCF();
			else if constexpr (Op::Y == 7)
				return CCF();
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte EI()
		{
			mInterruptsEnabled = true;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte DI()
		{
			mInterruptsEnabled = false;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte HALT()
		{
			mHalted = true;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte STOP()
		{
			mRegisters[WordReg::PC]++;

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte CB()
		{
			mCBInstruction = true;

			Byte instruction = AddressBus::Read(mRegisters[WordReg::PC]++);
			return CYCLES[0xCB] + mCBDispatcher[instruction]();
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte ASSERT() 
		{ 
			Emu::Assert(false, "Instruction not implemented!");

			return CYCLES[Op::Val];
		}

		// Instruction Implementations
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte NOP() 
		{
			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte ALU_T()
		{
			if constexpr (Op::Y == 0)
				return ADD_R<Op, T>();
			else if constexpr (Op::Y == 1)
				return ADC_R<Op, T>();
			else if constexpr (Op::Y == 2)
				return SUB_R<Op, T>();
			else if constexpr (Op::Y == 3)
				return SBC_R<Op, T>();
			else if constexpr (Op::Y == 4)
				return AND_R<Op, T>();
			else if constexpr (Op::Y == 5)
				return XOR_R<Op, T>();
			else if constexpr (Op::Y == 6)
				return OR_R<Op, T>();
			else if constexpr (Op::Y == 7)
				return CP_R<Op, T>();
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte ADD_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			Word result = regA + val;

			mFRegister.setZero((Byte)result == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(((regA & 0xF) + (val & 0xF)) & 0x10);
			mFRegister.setCarry(result & 0x100);

			regA = (Byte)result;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte ADC_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			Word result = regA + val + mFRegister.carry() ? 1 : 0;

			mFRegister.setZero((Byte)result == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(((regA & 0xF) + (val & 0xF)) & 0x10);
			mFRegister.setCarry(result & 0x100);

			regA = (Byte)result;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SUB_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			Word result = regA - val;

			mFRegister.setZero((Byte)result == 0);
			mFRegister.setSubtr(true);
			mFRegister.setHCarry(((regA & 0xF) - (val & 0xF)) & 0x10);
			mFRegister.setCarry(result & 0x100);

			regA = (Byte)result;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SBC_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			Word result = regA - val - mFRegister.carry() ? 1 : 0;

			mFRegister.setZero((Byte)result == 0);
			mFRegister.setSubtr(true);
			mFRegister.setHCarry(((regA & 0xF) - (val & 0xF)) & 0x10);
			mFRegister.setCarry(result & 0x100);

			regA = (Byte)result;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte AND_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			regA &= val;

			mFRegister.setZero(regA == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(true);
			mFRegister.setCarry(false);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte XOR_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			regA ^= val;

			mFRegister.setZero(regA == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(false);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte OR_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			ByteRef regA = mRegisters[ByteReg::A];
			regA |= val;

			mFRegister.setZero(regA == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(false);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte CP_R()
		{
			Byte val = [this]() -> Byte
			{
				if constexpr (Op::X == 3)
					return AddressBus::Read(mRegisters[WordReg::PC]++);

				return ReadByte<Op::Z>();
			}();

			Byte regA = mRegisters[ByteReg::A];
			SWord result = regA - val;

			mFRegister.setZero(result == 0);
			mFRegister.setSubtr(true);
			mFRegister.setHCarry(((regA & 0xF) - (val & 0xF)) & 0x10);
			mFRegister.setCarry(regA < val);

			return CYCLES[Op::Val];
		}

		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte INC()
		{
			Byte incVal = ReadByte<Op::Y>();
			Byte result = incVal + 1;
			WriteByte<Op::Y>(result);

			mFRegister.setZero(result == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(((incVal & 0xF) + 0x1) & 0x10);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte DEC()
		{
			Byte decVal = ReadByte<Op::Y>();
			Byte result = decVal - 1;
			WriteByte<Op::Y>(result);

			mFRegister.setZero(result == 0);
			mFRegister.setSubtr(true);
			mFRegister.setHCarry(((decVal & 0xF) - 0x1) & 0x10);

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte INC16()
		{
			WordRef reg = ReadWord<Op::P>();
			reg++;

			return CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte DEC16()
		{
			WordRef reg = ReadWord<Op::P>();
			reg--;

			return CYCLES[Op::Val];
		}

		Byte RLCA()
		{
			Byte regA = mRegisters[ByteReg::A];

			ByteBits regBits = regA;
			bool finalBit = regBits.test(7);

			regA <<= 1;
			regA |= finalBit ? GB_Bit(0) : 0;

			mRegisters[ByteReg::A] = regA;

			mFRegister.setZero(false);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(finalBit);

			return CYCLES[0x07];
		}
		Byte RRCA()
		{
			Byte regA = mRegisters[ByteReg::A];

			ByteBits regBits = regA;
			bool firstBit = regBits.test(0);

			regA >>= 1;
			regA |= firstBit ? GB_Bit(7) : 0;

			mRegisters[ByteReg::A] = regA;

			mFRegister.setZero(false);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(firstBit);

			return CYCLES[0x0F];
		}
		Byte RLA()
		{
			Byte regA = mRegisters[ByteReg::A];

			ByteBits regBits = regA;
			bool finalBit = regBits.test(7);

			bool setFirstBit = mFRegister.carry();

			regA <<= 1;
			regA |= setFirstBit ? GB_Bit(0) : 0;

			mRegisters[ByteReg::A] = regA;

			mFRegister.setZero(false);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(finalBit);

			return CYCLES[0x17];
		}
		Byte RRA()
		{
			Byte regA = mRegisters[ByteReg::A];

			ByteBits regBits = regA;
			bool firstBit = regBits.test(0);

			bool setFinalBit = mFRegister.carry();

			regA >>= 1;
			regA |= setFinalBit ? GB_Bit(7) : 0;

			mRegisters[ByteReg::A] = regA;

			mFRegister.setZero(false);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(firstBit);

			return CYCLES[0x1F];
		}
		Byte DAA()
		{
			ByteRef regA = mRegisters[ByteReg::A];

			Word correction = mFRegister.carry() ? 0x60 : 0x00;

			if (mFRegister.hcarry() || (!mFRegister.subtr() && ((regA & 0x0F) > 9)))
				correction |= 0x06;

			if (mFRegister.carry() || (!mFRegister.subtr() && (regA > 0x99)))
				correction |= 0x60;

			if (mFRegister.subtr())
				regA = (Byte)(regA - correction);
			else
				regA = (Byte)(regA + correction);

			mFRegister.setZero(regA == 0);
			mFRegister.setCarry((correction << 2) & 0x100);
			mFRegister.setHCarry(false);

			return CYCLES[0x27];
		}
		Byte CPL()
		{
			ByteRef regA = mRegisters[ByteReg::A];
			regA = ~regA;

			mFRegister.setSubtr(true);
			mFRegister.setHCarry(true);

			return CYCLES[0x2F];
		}
		Byte SCF()
		{
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(true);

			return CYCLES[0x37];
		}
		Byte CCF()
		{
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(!mFRegister.carry());

			return CYCLES[0x3F];
		}

		// CB Instructions
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RLC_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool finalBit = regBits.test(7);

			reg <<= 1;
			reg |= finalBit ? GB_Bit(0) : 0;

			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(finalBit);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RRC_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool firstBit = regBits.test(0);

			reg >>= 1;
			reg |= firstBit ? GB_Bit(7) : 0;

			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(firstBit);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RL_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool carryBit = regBits.test(7);
			bool setFirstBit = mFRegister.carry();

			reg <<= 1;
			reg |= setFirstBit ? GB_Bit(0) : 0;

			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(carryBit);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte RR_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool carryBit = regBits.test(0);
			bool setFinalBit = mFRegister.carry();

			reg >>= 1;
			reg |= setFinalBit ? GB_Bit(7) : 0;

			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(carryBit);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SLA_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool carryBit = regBits.test(7);

			reg <<= 1;
			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(carryBit);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SRA_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool finalBit = regBits.test(7);
			bool carryBit = regBits.test(0);

			reg >>= 1;
			reg |= finalBit ? GB_Bit(7) : 0;

			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(carryBit);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SWAP_R()
		{
			Byte reg = ReadByte<Op::Z>();
			Byte lo = (reg & 0x0F) << 4;
			Byte hi = (reg & 0xF0) >> 4;
			WriteByte<Op::Z>(hi | lo);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(false);

			return CB_CYCLES[Op::Val];
		}
		template<typename Op, Byte T> requires IsOpCode<Op, T>
		Byte SRL_R()
		{
			Byte reg = ReadByte<Op::Z>();

			ByteBits regBits = reg;
			bool carryBit = regBits.test(0);

			reg >>= 1;

			WriteByte<Op::Z>(reg);

			mFRegister.setZero(reg == 0);
			mFRegister.setSubtr(false);
			mFRegister.setHCarry(false);
			mFRegister.setCarry(carryBit);

			return CB_CYCLES[Op::Val];
		}

#pragma endregion

#pragma region OpCode Dispatchers

#define	GB_BIND_DISPATCH(func, op) [this] () -> Byte { return this->func<OpCode<op>, op>(); }

		using OpCodeFunction = std::function<Byte()>;
		const std::array<OpCodeFunction, 0x100> mDispatcher =
		{
			GB_BIND_DISPATCH(NOP, 0x00),		GB_BIND_DISPATCH(LD_R_I16, 0x01),	GB_BIND_DISPATCH(LD_M_R, 0x02),		GB_BIND_DISPATCH(UN_R, 0x03),
			GB_BIND_DISPATCH(UN_R, 0x04),		GB_BIND_DISPATCH(UN_R, 0x05),		GB_BIND_DISPATCH(LD_R_I8, 0x06),	GB_BIND_DISPATCH(REG, 0x07),
			GB_BIND_DISPATCH(LD_M_SP, 0x08),	GB_BIND_DISPATCH(ADD_HL, 0x09),		GB_BIND_DISPATCH(LD_R_M, 0x0A),		GB_BIND_DISPATCH(UN_R, 0x0B),
			GB_BIND_DISPATCH(UN_R, 0x0C),		GB_BIND_DISPATCH(UN_R, 0x0D),		GB_BIND_DISPATCH(LD_R_I8, 0x0E),	GB_BIND_DISPATCH(REG, 0x0F),
			GB_BIND_DISPATCH(STOP, 0x10),		GB_BIND_DISPATCH(LD_R_I16, 0x11),	GB_BIND_DISPATCH(LD_M_R, 0x12),		GB_BIND_DISPATCH(UN_R, 0x13),
			GB_BIND_DISPATCH(UN_R, 0x14),		GB_BIND_DISPATCH(UN_R, 0x15),		GB_BIND_DISPATCH(LD_R_I8, 0x16),	GB_BIND_DISPATCH(REG, 0x17),
			GB_BIND_DISPATCH(JP_C8, 0x18),		GB_BIND_DISPATCH(ADD_HL, 0x19),		GB_BIND_DISPATCH(LD_R_M, 0x1A),		GB_BIND_DISPATCH(UN_R, 0x1B),
			GB_BIND_DISPATCH(UN_R, 0x1C),		GB_BIND_DISPATCH(UN_R, 0x1D),		GB_BIND_DISPATCH(LD_R_I8, 0x1E),	GB_BIND_DISPATCH(REG, 0x1F),
			GB_BIND_DISPATCH(JP_C8, 0x20),		GB_BIND_DISPATCH(LD_R_I16, 0x21),	GB_BIND_DISPATCH(LD_M_R, 0x22),		GB_BIND_DISPATCH(UN_R, 0x23),
			GB_BIND_DISPATCH(UN_R, 0x24),		GB_BIND_DISPATCH(UN_R, 0x25),		GB_BIND_DISPATCH(LD_R_I8, 0x26),	GB_BIND_DISPATCH(REG, 0x27),
			GB_BIND_DISPATCH(JP_C8, 0x28),		GB_BIND_DISPATCH(ADD_HL, 0x29),		GB_BIND_DISPATCH(LD_R_M, 0x2A),		GB_BIND_DISPATCH(UN_R, 0x2B),
			GB_BIND_DISPATCH(UN_R, 0x2C),		GB_BIND_DISPATCH(UN_R, 0x2D),		GB_BIND_DISPATCH(LD_R_I8, 0x2E),	GB_BIND_DISPATCH(REG, 0x2F),
			GB_BIND_DISPATCH(JP_C8, 0x30),		GB_BIND_DISPATCH(LD_R_I16, 0x31),	GB_BIND_DISPATCH(LD_M_R, 0x32),		GB_BIND_DISPATCH(UN_R, 0x33),
			GB_BIND_DISPATCH(UN_R, 0x34),		GB_BIND_DISPATCH(UN_R, 0x35),		GB_BIND_DISPATCH(LD_R_I8, 0x36),	GB_BIND_DISPATCH(REG, 0x37),
			GB_BIND_DISPATCH(JP_C8, 0x38),		GB_BIND_DISPATCH(ADD_HL, 0x39),		GB_BIND_DISPATCH(LD_R_M, 0x3A),		GB_BIND_DISPATCH(UN_R, 0x3B),
			GB_BIND_DISPATCH(UN_R, 0x3C),		GB_BIND_DISPATCH(UN_R, 0x3D),		GB_BIND_DISPATCH(LD_R_I8, 0x3E),	GB_BIND_DISPATCH(REG, 0x3F),
			GB_BIND_DISPATCH(NOP, 0x40),		GB_BIND_DISPATCH(LD_R_R, 0x41),		GB_BIND_DISPATCH(LD_R_R, 0x42),		GB_BIND_DISPATCH(LD_R_R, 0x43),
			GB_BIND_DISPATCH(LD_R_R, 0x44),		GB_BIND_DISPATCH(LD_R_R, 0x45),		GB_BIND_DISPATCH(LD_R_R, 0x46),		GB_BIND_DISPATCH(LD_R_R, 0x47),
			GB_BIND_DISPATCH(LD_R_R, 0x48),		GB_BIND_DISPATCH(NOP, 0x49),		GB_BIND_DISPATCH(LD_R_R, 0x4A),		GB_BIND_DISPATCH(LD_R_R, 0x4B),
			GB_BIND_DISPATCH(LD_R_R, 0x4C),		GB_BIND_DISPATCH(LD_R_R, 0x4D),		GB_BIND_DISPATCH(LD_R_R, 0x4E),		GB_BIND_DISPATCH(LD_R_R, 0x4F),
			GB_BIND_DISPATCH(LD_R_R, 0x50),		GB_BIND_DISPATCH(LD_R_R, 0x51),		GB_BIND_DISPATCH(NOP, 0x52),		GB_BIND_DISPATCH(LD_R_R, 0x53),
			GB_BIND_DISPATCH(LD_R_R, 0x54),		GB_BIND_DISPATCH(LD_R_R, 0x55),		GB_BIND_DISPATCH(LD_R_R, 0x56),		GB_BIND_DISPATCH(LD_R_R, 0x57),
			GB_BIND_DISPATCH(LD_R_R, 0x58),		GB_BIND_DISPATCH(LD_R_R, 0x59),		GB_BIND_DISPATCH(LD_R_R, 0x5A),		GB_BIND_DISPATCH(NOP, 0x5B),
			GB_BIND_DISPATCH(LD_R_R, 0x5C),		GB_BIND_DISPATCH(LD_R_R, 0x5D),		GB_BIND_DISPATCH(LD_R_R, 0x5E),		GB_BIND_DISPATCH(LD_R_R, 0x5F),
			GB_BIND_DISPATCH(LD_R_R, 0x60),		GB_BIND_DISPATCH(LD_R_R, 0x61),		GB_BIND_DISPATCH(LD_R_R, 0x62),		GB_BIND_DISPATCH(LD_R_R, 0x63),
			GB_BIND_DISPATCH(NOP, 0x64),		GB_BIND_DISPATCH(LD_R_R, 0x65),		GB_BIND_DISPATCH(LD_R_R, 0x66),		GB_BIND_DISPATCH(LD_R_R, 0x67),
			GB_BIND_DISPATCH(LD_R_R, 0x68),		GB_BIND_DISPATCH(LD_R_R, 0x69),		GB_BIND_DISPATCH(LD_R_R, 0x6A),		GB_BIND_DISPATCH(LD_R_R, 0x6B),
			GB_BIND_DISPATCH(LD_R_R, 0x6C),		GB_BIND_DISPATCH(NOP, 0x6D),		GB_BIND_DISPATCH(LD_R_R, 0x6E),		GB_BIND_DISPATCH(LD_R_R, 0x6F),
			GB_BIND_DISPATCH(LD_R_R, 0x70),		GB_BIND_DISPATCH(LD_R_R, 0x71),		GB_BIND_DISPATCH(LD_R_R, 0x72),		GB_BIND_DISPATCH(LD_R_R, 0x73),
			GB_BIND_DISPATCH(LD_R_R, 0x74),		GB_BIND_DISPATCH(LD_R_R, 0x75),		GB_BIND_DISPATCH(HALT, 0x76),		GB_BIND_DISPATCH(LD_R_R, 0x77),
			GB_BIND_DISPATCH(LD_R_R, 0x78),		GB_BIND_DISPATCH(LD_R_R, 0x79),		GB_BIND_DISPATCH(LD_R_R, 0x7A),		GB_BIND_DISPATCH(LD_R_R, 0x7B),
			GB_BIND_DISPATCH(LD_R_R, 0x7C),		GB_BIND_DISPATCH(LD_R_R, 0x7D),		GB_BIND_DISPATCH(LD_R_R, 0x7E),		GB_BIND_DISPATCH(NOP, 0x7F),
			GB_BIND_DISPATCH(ALU_T, 0x80),		GB_BIND_DISPATCH(ALU_T, 0x81),		GB_BIND_DISPATCH(ALU_T, 0x82),		GB_BIND_DISPATCH(ALU_T, 0x83),
			GB_BIND_DISPATCH(ALU_T, 0x84),		GB_BIND_DISPATCH(ALU_T, 0x85),		GB_BIND_DISPATCH(ALU_T, 0x86),		GB_BIND_DISPATCH(ALU_T, 0x87),
			GB_BIND_DISPATCH(ALU_T, 0x88),		GB_BIND_DISPATCH(ALU_T, 0x89),		GB_BIND_DISPATCH(ALU_T, 0x8A),		GB_BIND_DISPATCH(ALU_T, 0x8B),
			GB_BIND_DISPATCH(ALU_T, 0x8C),		GB_BIND_DISPATCH(ALU_T, 0x8D),		GB_BIND_DISPATCH(ALU_T, 0x8E),		GB_BIND_DISPATCH(ALU_T, 0x8F),
			GB_BIND_DISPATCH(ALU_T, 0x90),		GB_BIND_DISPATCH(ALU_T, 0x91),		GB_BIND_DISPATCH(ALU_T, 0x92),		GB_BIND_DISPATCH(ALU_T, 0x93),
			GB_BIND_DISPATCH(ALU_T, 0x94),		GB_BIND_DISPATCH(ALU_T, 0x95),		GB_BIND_DISPATCH(ALU_T, 0x96),		GB_BIND_DISPATCH(ALU_T, 0x97),
			GB_BIND_DISPATCH(ALU_T, 0x98),		GB_BIND_DISPATCH(ALU_T, 0x99),		GB_BIND_DISPATCH(ALU_T, 0x9A),		GB_BIND_DISPATCH(ALU_T, 0x9B),
			GB_BIND_DISPATCH(ALU_T, 0x9C),		GB_BIND_DISPATCH(ALU_T, 0x9D),		GB_BIND_DISPATCH(ALU_T, 0x9E),		GB_BIND_DISPATCH(ALU_T, 0x9F),
			GB_BIND_DISPATCH(ALU_T, 0xA0),		GB_BIND_DISPATCH(ALU_T, 0xA1),		GB_BIND_DISPATCH(ALU_T, 0xA2),		GB_BIND_DISPATCH(ALU_T, 0xA3),
			GB_BIND_DISPATCH(ALU_T, 0xA4),		GB_BIND_DISPATCH(ALU_T, 0xA5),		GB_BIND_DISPATCH(ALU_T, 0xA6),		GB_BIND_DISPATCH(ALU_T, 0xA7),
			GB_BIND_DISPATCH(ALU_T, 0xA8),		GB_BIND_DISPATCH(ALU_T, 0xA9),		GB_BIND_DISPATCH(ALU_T, 0xAA),		GB_BIND_DISPATCH(ALU_T, 0xAB),
			GB_BIND_DISPATCH(ALU_T, 0xAC),		GB_BIND_DISPATCH(ALU_T, 0xAD),		GB_BIND_DISPATCH(ALU_T, 0xAE),		GB_BIND_DISPATCH(ALU_T, 0xAF),
			GB_BIND_DISPATCH(ALU_T, 0xB0),		GB_BIND_DISPATCH(ALU_T, 0xB1),		GB_BIND_DISPATCH(ALU_T, 0xB2),		GB_BIND_DISPATCH(ALU_T, 0xB3),
			GB_BIND_DISPATCH(ALU_T, 0xB4),		GB_BIND_DISPATCH(ALU_T, 0xB5),		GB_BIND_DISPATCH(ALU_T, 0xB6),		GB_BIND_DISPATCH(ALU_T, 0xB7),
			GB_BIND_DISPATCH(ALU_T, 0xB8),		GB_BIND_DISPATCH(ALU_T, 0xB9),		GB_BIND_DISPATCH(ALU_T, 0xBA),		GB_BIND_DISPATCH(ALU_T, 0xBB),
			GB_BIND_DISPATCH(ALU_T, 0xBC),		GB_BIND_DISPATCH(ALU_T, 0xBD),		GB_BIND_DISPATCH(ALU_T, 0xBE),		GB_BIND_DISPATCH(ALU_T, 0xBF),
			GB_BIND_DISPATCH(RET, 0xC0),		GB_BIND_DISPATCH(POP, 0xC1),		GB_BIND_DISPATCH(JP_C16, 0xC2),		GB_BIND_DISPATCH(JP_I16, 0xC3),
			GB_BIND_DISPATCH(CALL, 0xC4),		GB_BIND_DISPATCH(PUSH, 0xC5),		GB_BIND_DISPATCH(ALU_T, 0xC6),		GB_BIND_DISPATCH(ASSERT, 0xC7),
			GB_BIND_DISPATCH(RET, 0xC8),		GB_BIND_DISPATCH(RET, 0xC9),		GB_BIND_DISPATCH(JP_C16, 0xCA),		GB_BIND_DISPATCH(CB, 0xCB),
			GB_BIND_DISPATCH(CALL, 0xCC),		GB_BIND_DISPATCH(CALL, 0xCD),		GB_BIND_DISPATCH(ALU_T, 0xCE),		GB_BIND_DISPATCH(RST, 0xCF),
			GB_BIND_DISPATCH(RET, 0xD0),		GB_BIND_DISPATCH(POP, 0xD1),		GB_BIND_DISPATCH(JP_C16, 0xD2),		GB_BIND_DISPATCH(ASSERT, 0xD3),
			GB_BIND_DISPATCH(CALL, 0xD4),		GB_BIND_DISPATCH(PUSH, 0xD5),		GB_BIND_DISPATCH(ALU_T, 0xD6),		GB_BIND_DISPATCH(ASSERT, 0xD7),
			GB_BIND_DISPATCH(RET, 0xD8),		GB_BIND_DISPATCH(RETI, 0xD9),		GB_BIND_DISPATCH(JP_C16, 0xDA),		GB_BIND_DISPATCH(ASSERT, 0xDB),
			GB_BIND_DISPATCH(CALL, 0xDC),		GB_BIND_DISPATCH(ASSERT, 0xDD),		GB_BIND_DISPATCH(ALU_T, 0xDE),		GB_BIND_DISPATCH(RST, 0xDF),
			GB_BIND_DISPATCH(LD_R_STK, 0xE0),	GB_BIND_DISPATCH(POP, 0xE1),		GB_BIND_DISPATCH(LD_R_STK, 0xE2),	GB_BIND_DISPATCH(ASSERT, 0xE3),
			GB_BIND_DISPATCH(ASSERT, 0xE4),		GB_BIND_DISPATCH(PUSH, 0xE5),		GB_BIND_DISPATCH(ALU_T, 0xE6),		GB_BIND_DISPATCH(ASSERT, 0xE7),
			GB_BIND_DISPATCH(ASSERT, 0xE8),		GB_BIND_DISPATCH(LD_HL_PC, 0xE9),	GB_BIND_DISPATCH(LD_R_STK16, 0xEA),	GB_BIND_DISPATCH(ASSERT, 0xEB),
			GB_BIND_DISPATCH(ASSERT, 0xEC),		GB_BIND_DISPATCH(ASSERT, 0xED),		GB_BIND_DISPATCH(ALU_T, 0xEE),		GB_BIND_DISPATCH(RST, 0xEF),
			GB_BIND_DISPATCH(LD_R_STK, 0xF0),	GB_BIND_DISPATCH(POP, 0xF1),		GB_BIND_DISPATCH(LD_R_STK, 0xF2),	GB_BIND_DISPATCH(DI, 0xF3),
			GB_BIND_DISPATCH(ASSERT, 0xF4),		GB_BIND_DISPATCH(PUSH, 0xF5),		GB_BIND_DISPATCH(ALU_T, 0xF6),		GB_BIND_DISPATCH(ASSERT, 0xF7),
			GB_BIND_DISPATCH(LD_HL_SP, 0xF8),	GB_BIND_DISPATCH(LD_HL_SP16, 0xF9),	GB_BIND_DISPATCH(LD_R_STK16, 0xFA),	GB_BIND_DISPATCH(EI, 0xFB),
			GB_BIND_DISPATCH(ASSERT, 0xFC),		GB_BIND_DISPATCH(ASSERT, 0xFD),		GB_BIND_DISPATCH(ALU_T, 0xFE),		GB_BIND_DISPATCH(RST, 0xFF)
		};

		const std::array<OpCodeFunction, 0x100> mCBDispatcher =
		{
			GB_BIND_DISPATCH(ROT_R, 0x00),		GB_BIND_DISPATCH(ROT_R, 0x01),		GB_BIND_DISPATCH(ROT_R, 0x02),		GB_BIND_DISPATCH(ROT_R, 0x03),
			GB_BIND_DISPATCH(ROT_R, 0x04),		GB_BIND_DISPATCH(ROT_R, 0x05),		GB_BIND_DISPATCH(ROT_R, 0x06),		GB_BIND_DISPATCH(ROT_R, 0x07),
			GB_BIND_DISPATCH(ROT_R, 0x08),		GB_BIND_DISPATCH(ROT_R, 0x09),		GB_BIND_DISPATCH(ROT_R, 0x0A),		GB_BIND_DISPATCH(ROT_R, 0x0B),
			GB_BIND_DISPATCH(ROT_R, 0x0C),		GB_BIND_DISPATCH(ROT_R, 0x0D),		GB_BIND_DISPATCH(ROT_R, 0x0E),		GB_BIND_DISPATCH(ROT_R, 0x0F),
			GB_BIND_DISPATCH(ROT_R, 0x10),		GB_BIND_DISPATCH(ROT_R, 0x11),		GB_BIND_DISPATCH(ROT_R, 0x12),		GB_BIND_DISPATCH(ROT_R, 0x13),
			GB_BIND_DISPATCH(ROT_R, 0x14),		GB_BIND_DISPATCH(ROT_R, 0x15),		GB_BIND_DISPATCH(ROT_R, 0x16),		GB_BIND_DISPATCH(ROT_R, 0x17),
			GB_BIND_DISPATCH(ROT_R, 0x18),		GB_BIND_DISPATCH(ROT_R, 0x19),		GB_BIND_DISPATCH(ROT_R, 0x1A),		GB_BIND_DISPATCH(ROT_R, 0x1B),
			GB_BIND_DISPATCH(ROT_R, 0x1C),		GB_BIND_DISPATCH(ROT_R, 0x1D),		GB_BIND_DISPATCH(ROT_R, 0x1E),		GB_BIND_DISPATCH(ROT_R, 0x1F),
			GB_BIND_DISPATCH(ROT_R, 0x20),		GB_BIND_DISPATCH(ROT_R, 0x21),		GB_BIND_DISPATCH(ROT_R, 0x22),		GB_BIND_DISPATCH(ROT_R, 0x23),
			GB_BIND_DISPATCH(ROT_R, 0x24),		GB_BIND_DISPATCH(ROT_R, 0x25),		GB_BIND_DISPATCH(ROT_R, 0x26),		GB_BIND_DISPATCH(ROT_R, 0x27),
			GB_BIND_DISPATCH(ROT_R, 0x28),		GB_BIND_DISPATCH(ROT_R, 0x29),		GB_BIND_DISPATCH(ROT_R, 0x2A),		GB_BIND_DISPATCH(ROT_R, 0x2B),
			GB_BIND_DISPATCH(ROT_R, 0x2C),		GB_BIND_DISPATCH(ROT_R, 0x2D),		GB_BIND_DISPATCH(ROT_R, 0x2E),		GB_BIND_DISPATCH(ROT_R, 0x2F),
			GB_BIND_DISPATCH(ROT_R, 0x30),		GB_BIND_DISPATCH(ROT_R, 0x31),		GB_BIND_DISPATCH(ROT_R, 0x32),		GB_BIND_DISPATCH(ROT_R, 0x33),
			GB_BIND_DISPATCH(ROT_R, 0x34),		GB_BIND_DISPATCH(ROT_R, 0x35),		GB_BIND_DISPATCH(ROT_R, 0x36),		GB_BIND_DISPATCH(ROT_R, 0x37),
			GB_BIND_DISPATCH(ROT_R, 0x38),		GB_BIND_DISPATCH(ROT_R, 0x39),		GB_BIND_DISPATCH(ROT_R, 0x3A),		GB_BIND_DISPATCH(ROT_R, 0x3B),
			GB_BIND_DISPATCH(ROT_R, 0x3C),		GB_BIND_DISPATCH(ROT_R, 0x3D),		GB_BIND_DISPATCH(ROT_R, 0x3E),		GB_BIND_DISPATCH(ROT_R, 0x3F),
			GB_BIND_DISPATCH(BIT_R, 0x40),		GB_BIND_DISPATCH(BIT_R, 0x41),		GB_BIND_DISPATCH(BIT_R, 0x42),		GB_BIND_DISPATCH(BIT_R, 0x43),
			GB_BIND_DISPATCH(BIT_R, 0x44),		GB_BIND_DISPATCH(BIT_R, 0x45),		GB_BIND_DISPATCH(BIT_R, 0x46),		GB_BIND_DISPATCH(BIT_R, 0x47),
			GB_BIND_DISPATCH(BIT_R, 0x48),		GB_BIND_DISPATCH(BIT_R, 0x49),		GB_BIND_DISPATCH(BIT_R, 0x4A),		GB_BIND_DISPATCH(BIT_R, 0x4B),
			GB_BIND_DISPATCH(BIT_R, 0x4C),		GB_BIND_DISPATCH(BIT_R, 0x4D),		GB_BIND_DISPATCH(BIT_R, 0x4E),		GB_BIND_DISPATCH(BIT_R, 0x4F),
			GB_BIND_DISPATCH(BIT_R, 0x50),		GB_BIND_DISPATCH(BIT_R, 0x51),		GB_BIND_DISPATCH(BIT_R, 0x52),		GB_BIND_DISPATCH(BIT_R, 0x53),
			GB_BIND_DISPATCH(BIT_R, 0x54),		GB_BIND_DISPATCH(BIT_R, 0x55),		GB_BIND_DISPATCH(BIT_R, 0x56),		GB_BIND_DISPATCH(BIT_R, 0x57),
			GB_BIND_DISPATCH(BIT_R, 0x58),		GB_BIND_DISPATCH(BIT_R, 0x59),		GB_BIND_DISPATCH(BIT_R, 0x5A),		GB_BIND_DISPATCH(BIT_R, 0x5B),
			GB_BIND_DISPATCH(BIT_R, 0x5C),		GB_BIND_DISPATCH(BIT_R, 0x5D),		GB_BIND_DISPATCH(BIT_R, 0x5E),		GB_BIND_DISPATCH(BIT_R, 0x5F),
			GB_BIND_DISPATCH(BIT_R, 0x60),		GB_BIND_DISPATCH(BIT_R, 0x61),		GB_BIND_DISPATCH(BIT_R, 0x62),		GB_BIND_DISPATCH(BIT_R, 0x63),
			GB_BIND_DISPATCH(BIT_R, 0x64),		GB_BIND_DISPATCH(BIT_R, 0x65),		GB_BIND_DISPATCH(BIT_R, 0x66),		GB_BIND_DISPATCH(BIT_R, 0x67),
			GB_BIND_DISPATCH(BIT_R, 0x68),		GB_BIND_DISPATCH(BIT_R, 0x69),		GB_BIND_DISPATCH(BIT_R, 0x6A),		GB_BIND_DISPATCH(BIT_R, 0x6B),
			GB_BIND_DISPATCH(BIT_R, 0x6C),		GB_BIND_DISPATCH(BIT_R, 0x6D),		GB_BIND_DISPATCH(BIT_R, 0x6E),		GB_BIND_DISPATCH(BIT_R, 0x6F),
			GB_BIND_DISPATCH(BIT_R, 0x70),		GB_BIND_DISPATCH(BIT_R, 0x71),		GB_BIND_DISPATCH(BIT_R, 0x72),		GB_BIND_DISPATCH(BIT_R, 0x73),
			GB_BIND_DISPATCH(BIT_R, 0x74),		GB_BIND_DISPATCH(BIT_R, 0x75),		GB_BIND_DISPATCH(BIT_R, 0x76),		GB_BIND_DISPATCH(BIT_R, 0x77),
			GB_BIND_DISPATCH(BIT_R, 0x78),		GB_BIND_DISPATCH(BIT_R, 0x79),		GB_BIND_DISPATCH(BIT_R, 0x7A),		GB_BIND_DISPATCH(BIT_R, 0x7B),
			GB_BIND_DISPATCH(BIT_R, 0x7C),		GB_BIND_DISPATCH(BIT_R, 0x7D),		GB_BIND_DISPATCH(BIT_R, 0x7E),		GB_BIND_DISPATCH(BIT_R, 0x7F),
			GB_BIND_DISPATCH(RES_R, 0x80),		GB_BIND_DISPATCH(RES_R, 0x81),		GB_BIND_DISPATCH(RES_R, 0x82),		GB_BIND_DISPATCH(RES_R, 0x83),
			GB_BIND_DISPATCH(RES_R, 0x84),		GB_BIND_DISPATCH(RES_R, 0x85),		GB_BIND_DISPATCH(RES_R, 0x86),		GB_BIND_DISPATCH(RES_R, 0x87),
			GB_BIND_DISPATCH(RES_R, 0x88),		GB_BIND_DISPATCH(RES_R, 0x89),		GB_BIND_DISPATCH(RES_R, 0x8A),		GB_BIND_DISPATCH(RES_R, 0x8B),
			GB_BIND_DISPATCH(RES_R, 0x8C),		GB_BIND_DISPATCH(RES_R, 0x8D),		GB_BIND_DISPATCH(RES_R, 0x8E),		GB_BIND_DISPATCH(RES_R, 0x8F),
			GB_BIND_DISPATCH(RES_R, 0x90),		GB_BIND_DISPATCH(RES_R, 0x91),		GB_BIND_DISPATCH(RES_R, 0x92),		GB_BIND_DISPATCH(RES_R, 0x93),
			GB_BIND_DISPATCH(RES_R, 0x94),		GB_BIND_DISPATCH(RES_R, 0x95),		GB_BIND_DISPATCH(RES_R, 0x96),		GB_BIND_DISPATCH(RES_R, 0x97),
			GB_BIND_DISPATCH(RES_R, 0x98),		GB_BIND_DISPATCH(RES_R, 0x99),		GB_BIND_DISPATCH(RES_R, 0x9A),		GB_BIND_DISPATCH(RES_R, 0x9B),
			GB_BIND_DISPATCH(RES_R, 0x9C),		GB_BIND_DISPATCH(RES_R, 0x9D),		GB_BIND_DISPATCH(RES_R, 0x9E),		GB_BIND_DISPATCH(RES_R, 0x9F),
			GB_BIND_DISPATCH(RES_R, 0xA0),		GB_BIND_DISPATCH(RES_R, 0xA1),		GB_BIND_DISPATCH(RES_R, 0xA2),		GB_BIND_DISPATCH(RES_R, 0xA3),
			GB_BIND_DISPATCH(RES_R, 0xA4),		GB_BIND_DISPATCH(RES_R, 0xA5),		GB_BIND_DISPATCH(RES_R, 0xA6),		GB_BIND_DISPATCH(RES_R, 0xA7),
			GB_BIND_DISPATCH(RES_R, 0xA8),		GB_BIND_DISPATCH(RES_R, 0xA9),		GB_BIND_DISPATCH(RES_R, 0xAA),		GB_BIND_DISPATCH(RES_R, 0xAB),
			GB_BIND_DISPATCH(RES_R, 0xAC),		GB_BIND_DISPATCH(RES_R, 0xAD),		GB_BIND_DISPATCH(RES_R, 0xAE),		GB_BIND_DISPATCH(RES_R, 0xAF),
			GB_BIND_DISPATCH(RES_R, 0xB0),		GB_BIND_DISPATCH(RES_R, 0xB1),		GB_BIND_DISPATCH(RES_R, 0xB2),		GB_BIND_DISPATCH(RES_R, 0xB3),
			GB_BIND_DISPATCH(RES_R, 0xB4),		GB_BIND_DISPATCH(RES_R, 0xB5),		GB_BIND_DISPATCH(RES_R, 0xB6),		GB_BIND_DISPATCH(RES_R, 0xB7),
			GB_BIND_DISPATCH(RES_R, 0xB8),		GB_BIND_DISPATCH(RES_R, 0xB9),		GB_BIND_DISPATCH(RES_R, 0xBA),		GB_BIND_DISPATCH(RES_R, 0xBB),
			GB_BIND_DISPATCH(RES_R, 0xBC),		GB_BIND_DISPATCH(RES_R, 0xBD),		GB_BIND_DISPATCH(RES_R, 0xBE),		GB_BIND_DISPATCH(RES_R, 0xBF),
			GB_BIND_DISPATCH(SET_R, 0xC0),		GB_BIND_DISPATCH(SET_R, 0xC1),		GB_BIND_DISPATCH(SET_R, 0xC2),		GB_BIND_DISPATCH(SET_R, 0xC3),
			GB_BIND_DISPATCH(SET_R, 0xC4),		GB_BIND_DISPATCH(SET_R, 0xC5),		GB_BIND_DISPATCH(SET_R, 0xC6),		GB_BIND_DISPATCH(SET_R, 0xC7),
			GB_BIND_DISPATCH(SET_R, 0xC8),		GB_BIND_DISPATCH(SET_R, 0xC9),		GB_BIND_DISPATCH(SET_R, 0xCA),		GB_BIND_DISPATCH(SET_R, 0xCB),
			GB_BIND_DISPATCH(SET_R, 0xCC),		GB_BIND_DISPATCH(SET_R, 0xCD),		GB_BIND_DISPATCH(SET_R, 0xCE),		GB_BIND_DISPATCH(SET_R, 0xCF),
			GB_BIND_DISPATCH(SET_R, 0xD0),		GB_BIND_DISPATCH(SET_R, 0xD1),		GB_BIND_DISPATCH(SET_R, 0xD2),		GB_BIND_DISPATCH(SET_R, 0xD3),
			GB_BIND_DISPATCH(SET_R, 0xD4),		GB_BIND_DISPATCH(SET_R, 0xD5),		GB_BIND_DISPATCH(SET_R, 0xD6),		GB_BIND_DISPATCH(SET_R, 0xD7),
			GB_BIND_DISPATCH(SET_R, 0xD8),		GB_BIND_DISPATCH(SET_R, 0xD9),		GB_BIND_DISPATCH(SET_R, 0xDA),		GB_BIND_DISPATCH(SET_R, 0xDB),
			GB_BIND_DISPATCH(SET_R, 0xDC),		GB_BIND_DISPATCH(SET_R, 0xDD),		GB_BIND_DISPATCH(SET_R, 0xDE),		GB_BIND_DISPATCH(SET_R, 0xDF),
			GB_BIND_DISPATCH(SET_R, 0xE0),		GB_BIND_DISPATCH(SET_R, 0xE1),		GB_BIND_DISPATCH(SET_R, 0xE2),		GB_BIND_DISPATCH(SET_R, 0xE3),
			GB_BIND_DISPATCH(SET_R, 0xE4),		GB_BIND_DISPATCH(SET_R, 0xE5),		GB_BIND_DISPATCH(SET_R, 0xE6),		GB_BIND_DISPATCH(SET_R, 0xE7),
			GB_BIND_DISPATCH(SET_R, 0xE8),		GB_BIND_DISPATCH(SET_R, 0xE9),		GB_BIND_DISPATCH(SET_R, 0xEA),		GB_BIND_DISPATCH(SET_R, 0xEB),
			GB_BIND_DISPATCH(SET_R, 0xEC),		GB_BIND_DISPATCH(SET_R, 0xED),		GB_BIND_DISPATCH(SET_R, 0xEE),		GB_BIND_DISPATCH(SET_R, 0xEF),
			GB_BIND_DISPATCH(SET_R, 0xF0),		GB_BIND_DISPATCH(SET_R, 0xF1),		GB_BIND_DISPATCH(SET_R, 0xF2),		GB_BIND_DISPATCH(SET_R, 0xF3),
			GB_BIND_DISPATCH(SET_R, 0xF4),		GB_BIND_DISPATCH(SET_R, 0xF5),		GB_BIND_DISPATCH(SET_R, 0xF6),		GB_BIND_DISPATCH(SET_R, 0xF7),
			GB_BIND_DISPATCH(SET_R, 0xF8),		GB_BIND_DISPATCH(SET_R, 0xF9),		GB_BIND_DISPATCH(SET_R, 0xFA),		GB_BIND_DISPATCH(SET_R, 0xFB),
			GB_BIND_DISPATCH(SET_R, 0xFC),		GB_BIND_DISPATCH(SET_R, 0xFD),		GB_BIND_DISPATCH(SET_R, 0xFE),		GB_BIND_DISPATCH(SET_R, 0xFF)
		};

#pragma endregion
	};
}
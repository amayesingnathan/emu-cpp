#include "gbpch.h"
#include "CPU.h"

#include "Instructions.h"

namespace GB {

	int CPU::exec()
	{
		if (mHalted)
			return 0;

		OpCode nextOp = AddressBus::Read(mRegisters[WordReg::PC]++);
		return HandleInstruction(nextOp);
	}

	bool CPU::CheckCondition(Condition condition)
	{
		bool shouldBranch = false;

		switch (condition) {
		case Condition::C:
			shouldBranch = mFRegister.carry();
			break;
		case Condition::NC:
			shouldBranch = !mFRegister.carry();
			break;
		case Condition::Z:
			shouldBranch = mFRegister.zero();
			break;
		case Condition::NZ:
			shouldBranch = !mFRegister.zero();
			break;
		}

		/* If the branch is taken, remember so that the correct processor cycles
		 * can be used */
		mBranchTaken = shouldBranch;
		return shouldBranch;
	}

	int CPU::HandleInstruction(OpCode instruction)
	{
		switch (instruction.data)
		{
		// NOP or effective NOPs
		case 0x00: case 0x40: case 0x49: case 0x52: case 0x5B: case 0x64: case 0x6D: case 0x7F:
			break; 

		// Register-16bit immediate load
		case 0x01: case 0x11: case 0x21: case 0x31:
		{
			Word lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			Word hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			mRegisters[sRegisterPairs[instruction.p()]] = lo | hi << 8;
			break;
		}

		// Register-Memory load
		case 0x02: case 0x12: case 0x22: case 0x32:
		{
			LD_M_R(instruction);
			break;
		}

		// Memory-Register load
		case 0x0A: case 0x1A: case 0x2A: case 0x3A:
		{
			LD_R_M(instruction);
			break;
		}

		// Unary Inc/Dec
		case 0x03: case 0x13: case 0x23: case 0x33: case 0x0B: case 0x1B: case 0x2B: case 0x3B:
		case 0x04: case 0x14: case 0x24: case 0x34: case 0x0C: case 0x1C: case 0x2C: case 0x3C:
		case 0x05: case 0x15: case 0x25: case 0x35: case 0x0D: case 0x1D: case 0x2D: case 0x3D:
		{
			UN_R(instruction);
			break;
		}

		// Accumulator/Flag Ops
		case 0x07: case 0x0F: case 0x17: case 0x1F: case 0x27: case 0x2F: case 0x37: case 0x3F:
		{
			REG(instruction);
			break;
		}

		// Register-8bit immediate load
		case 0x06: case 0x16: case 0x26: case 0x36:
		case 0x0E: case 0x1E: case 0x2E: case 0x3E:
		{
			LD_R_I8(instruction);
			break;
		}

		// 8bit immediate Jump/Conditional Jumps
		case 0x18: case 0x20: case 0x28: case 0x30: case 0x38:
			JR_C8(instruction);
			break;

		// 8 bit Register-Register load
		case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
		case 0x50: case 0x51: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5A: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6E: case 0x6F:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
		case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E:
		{
			LD_R_R(instruction);
			break;
		}
		case 0x76:
			mHalted = true;
			break;

		// Register-Register Arithmetic/Logic
		case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
		case 0x90: case 0x91: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9A: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
		case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA5: case 0xA6: case 0xA7:
		case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAE: case 0xAF:
		case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7:
		case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE:
		{
			Byte value;
			READ_REG(instruction.z(), value);
			ALU_T(value, instruction.y());
			break;
		}

		// 8 bit immediate Arithmetic/Logic
		case 0xC6: case 0xCE: case 0xD6: case 0xDE: case 0xE6: case 0xEE: case 0xF6: case 0xFE:
		{
			Byte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
			ALU_T(imm, instruction.y());
			break;
		}

		// Stack Pop
		case 0xC1: case 0xD1: case 0xE1:case 0xF1:
		{
			POP(instruction.p());
			break;
		}

		// Stack Push
		case 0xC5: case 0xD5: case 0xE5:case 0xF5:
		{
			PUSH(instruction.p());
			break;
		}

		// Calls
		case 0xC4: case 0xD4: case 0xCC: case 0xDC: case 0xCD:
		{
			CALL(instruction);
			break;
		}

		// Returns
		case 0xC0: case 0xD0: case 0xC8: case 0xD8: case 0xC9:
		{
			RET(instruction);
			break;
		}


		// Load to/from WRAM/IO
		case 0xE0: case 0xE2: case 0xF0: case 0xF2:
		{
			LD_R_STK(instruction);
			break;
		}

		// CB Instruction Prefix
		case 0xCB: 
			return HandleCBInstruction();

		// Disable Master Interrupt
		case 0xF3:
			mInteruptsEnabled = false;
			break;

		// Enable Master Interrupt
		case 0xFB:
			mInteruptsEnabled = true;
			break;

		default: GB_ASSERT(false, "Instruction not yet implemented!");
		}

		return sCycles[instruction];
	}

	int CPU::HandleCBInstruction()
	{
		OpCode instruction = AddressBus::Read(mRegisters[WordReg::PC]++);
		switch (instruction.x())
		{
		case 0: ROT_R(instruction); break;
		case 1: BIT_R(instruction); break;
		case 2: RES_R(instruction); break;
		case 3: SET_R(instruction); break;
		default: break;
		}

		return sCBCycles[instruction];
	}

#pragma region OpCodeFunctions
	void CPU::LD_R_R(OpCode op)
	{
		Byte src;
		READ_REG(op.z(), src);
		WRITE_REG(op.y(), src);
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
		src = (Byte)((SByte)src + incDec);

		Word& address = mRegisters[sRegisterPairs[p]];
		AddressBus::Write(address, src);
		address += incDec;
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
	}

	void CPU::LD_R_I8(OpCode op)
	{
		Byte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
		WRITE_REG(op.y(), imm);
	}

	void CPU::LD_R_STK(OpCode op)
	{
		Byte addr = (op.z() & GB_BIT(1)) ? mRegisters[ByteReg::C] : AddressBus::Read(mRegisters[WordReg::PC]++);

		if (op & GB_BIT(3))
			mRegisters[ByteReg::A] = AddressBus::Read(0xFF00 + (Word)addr);
		else
			AddressBus::Write(0xFF00 + (Word)addr, mRegisters[ByteReg::A]);

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

	void CPU::BIT_R(OpCode op)
	{
		Byte value;
		READ_REG(op.z(), value);

		Flag flags = mFRegister.getFlags();
		flags &= ~_SubtractFlag;
		flags |= _HCarryFlag;
		if (value & GB_BIT(op.y()))
			flags &= ~_ZeroFlag;
		else
			flags |= _ZeroFlag;
		mFRegister.setFlags(flags);
	}

	void CPU::RES_R(OpCode op)
	{
		Byte targetReg = op.z();
		Byte bit = GB_BIT(op.y());
		Byte value;
		READ_REG(targetReg, value);
		value &= ~bit;
		WRITE_REG(targetReg, value);
	}

	void CPU::SET_R(OpCode op)
	{
		Byte targetReg = op.z();
		Byte bit = GB_BIT(op.y());
		Byte value;
		READ_REG(targetReg, value);
		value |= bit;
		WRITE_REG(targetReg, value);
	}

	void CPU::JR_C8(OpCode op)
	{
		SByte imm = AddressBus::Read(mRegisters[WordReg::PC]++);
		Byte condTarget = op.y();

		bool hasCondition = (condTarget & 0x04);
		if (hasCondition) 
		{
			if (CheckCondition((Condition)(condTarget - 0x04)))
				mRegisters[WordReg::PC] += imm;
			return;
		}

		mRegisters[WordReg::PC] += imm;
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
		Word imm = lo | hi << 8;

		// Condition not met
		if (!(op & 0x01) && !CheckCondition((Condition)op.y()))
			return;

		Word& pc = mRegisters[WordReg::PC];
		Word& sp = mRegisters[WordReg::SP];
		AddressBus::Write(--sp, (Byte)(pc >> 8));
		AddressBus::Write(--sp, (Byte)(pc & 0x00FF));
		pc = imm;
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
	}

	void CPU::REG(OpCode op)
	{
		switch (op)
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



	// Instruction Implementations

	void CPU::ALU_T(Byte value, Byte op)
	{
		switch (op)
		{
		case 0: ADD_R(value); break;
		case 1: ADC_R(value); break;
		case 2: SUB_R(value); break;
		case 3: SBC_R(value); break;
		case 4: AND_R(value); break;
		case 5: XOR_R(value); break;
		case 6: OR_R(value); break;
		case 7: CP_R(value); break;
		}
	}

	void CPU::ADD_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Word result = regA + value;
		regA = (Byte)result;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? _HCarryFlag : 0;
		flags |= ((result & 0x100) != 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::ADC_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Word result = regA + value + (mFRegister.carry() ? 1 : 0);
		regA = (Byte)result;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? _HCarryFlag : 0;
		flags |= ((result & 0x100) != 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::SUB_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Byte result = regA - value;
		regA = result;

		Flag flags = _SubtractFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? _HCarryFlag : 0;
		flags |= ((result & 0x100) != 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::SBC_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;

		Byte carry = mFRegister.carry() ? 1 : 0;
		SWord result = regA - value - carry;
		regA = (Byte)result;

		Flag flags = _SubtractFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) - (value & 0xf) - carry) ? _HCarryFlag : 0;
		flags |= (result < 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::AND_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];

		regA &= value;

		Flag flags = _HCarryFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::XOR_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];

		regA ^= value;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::OR_R(Byte value)
	{
		Byte& regA = mRegisters[ByteReg::A];

		regA |= value;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::CP_R(Byte value)
	{
		Byte regA = mRegisters[ByteReg::A];

		Byte result = regA - value;

		Flag flags = _SubtractFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regA & 0xf) - (value & 0xf) < 0) ? _HCarryFlag : 0;
		flags |= (regA < value) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::PUSH(Byte target)
	{
		Word& sp = mRegisters[WordReg::SP];
		Word regPair = mRegisters[sRegisterPairs2[target]];

		AddressBus::Write(--sp, (Byte)(regPair >> 8));
		AddressBus::Write(--sp, (Byte)(regPair & 0x00FF));
	}

	void CPU::POP(Byte target)
	{
		Word& sp = mRegisters[WordReg::SP];
		Word& regPair = mRegisters[sRegisterPairs2[target]];

		regPair = (Word)AddressBus::Read(sp++);
		regPair |= (Word)AddressBus::Read(sp++) << 8;
	}

	void CPU::INC(Byte target)
	{
		Byte reg;
		READ_REG(target, reg);
		reg++;
		WRITE_REG(target, reg);

		Flag flags = mFRegister.getFlags();
		flags |= (reg == 0) ? _ZeroFlag : 0;
		flags &= ~_SubtractFlag;
		flags |= ((reg & 0x0F) == 0x00) ? _HCarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::DEC(Byte target)
	{
		Byte reg;
		READ_REG(target, reg);
		reg--;
		WRITE_REG(target, reg);

		Flag flags = mFRegister.getFlags();
		flags |= (reg == 0) ? _ZeroFlag : 0;
		flags |= _SubtractFlag;
		flags |= ((reg & 0x0F) == 0x0F) ? _HCarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::INC16(Byte target)
	{
		Word& reg = mRegisters[sRegisterPairs[target]];
		reg++;
	}

	void CPU::DEC16(Byte target)
	{
		Word& reg = mRegisters[sRegisterPairs[target]];
		reg--;
	}

	void CPU::RLCA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte finalBit = (regA & GB_BIT(7)) ? GB_BIT(0) : 0;
		regA = (Byte)(regA << 1) | finalBit;

		Flag flags = 0;
		flags |= regA == 0 ? _ZeroFlag : 0;
		flags |= finalBit ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::RRCA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte firstBit = (regA & GB_BIT(0)) ? GB_BIT(7) : 0;
		regA = (Byte)(regA >> 1) | firstBit;

		Flag flags = 0;
		flags |= regA == 0 ? _ZeroFlag : 0;
		flags |= firstBit ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::RLA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte firstBit = mFRegister.carry() ? GB_BIT(0) : 0;
		regA = (Byte)(regA << 1) | firstBit;

		Flag flags = mFRegister.getFlags();
		flags &= ~_SubtractFlag;
		flags &= ~ _HCarryFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::RRA()
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte finalBit = mFRegister.carry() ? GB_BIT(7) : 0;
		regA = (Byte)(regA >> 1) | finalBit;

		Flag flags = mFRegister.getFlags();
		flags &= ~_SubtractFlag;
		flags &= ~_HCarryFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
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

		Flag flags = mFRegister.subtr() ? _SubtractFlag : 0;
		if (((correction << 2) & 0x100) != 0)
			flags |= _CarryFlag;

		flags &= ~_HCarryFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
	}

	void CPU::CPL()
	{
		Byte& regA = mRegisters[ByteReg::A];
		regA = ~regA;

		Flag flags = mFRegister.getFlags();
		flags |= _SubtractFlag | _HCarryFlag;
		mFRegister.setFlags(flags);
	}

	void CPU::SCF()
	{
		Flag flags = mFRegister.getFlags();
		flags &= ~_SubtractFlag & ~_HCarryFlag;
		flags |= _CarryFlag;
		mFRegister.setFlags(flags);
	}

	void CPU::CCF()
	{
		Flag flags = mFRegister.getFlags();
		flags &= ~_SubtractFlag & ~_HCarryFlag;
		flags |= !(flags & _CarryFlag) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	// CB Instructions Implementations

	void CPU::RLC_R(Byte target)
	{
	}

	void CPU::RRC_R(Byte target)
	{
	}

	void CPU::RL_R(Byte target)
	{
	}

	void CPU::RR_R(Byte target)
	{
	}

	void CPU::SLA_R(Byte target)
	{
	}

	void CPU::SRA_R(Byte target)
	{
	}

	void CPU::SWAP_R(Byte target)
	{
	}

	void CPU::SRL_R(Byte target)
	{
	}

#pragma endregion
}

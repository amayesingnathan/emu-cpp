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

	bool CPU::IsCondition(Condition condition)
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

		// 16 bit register load
		case 0x01: case 0x11: case 0x21: case 0x31:
		{
			Register imm;
			imm.hi = AddressBus::Read(mRegisters[WordReg::PC]++);
			imm.lo = AddressBus::Read(mRegisters[WordReg::PC]++);
			mRegisters[sRegisterPairs[instruction.p()]] = imm.dreg;
			break;
		}

		// Register-memory load
		case 0x02: case 0x12: case 0x22: case 0x32:
		{
			Byte p = instruction.p();
			SByte incDec = 0;
			if (p > 1)
				incDec = -1 * ((SByte)(p * 2) - 5);
			if (p == 3)
				p--;

			Byte src;
			READ_REG(7, src);
			src = (Byte)((SByte)src + incDec);

			Word& address = mRegisters[sRegisterPairs[p]];
			AddressBus::Write(address, src);
			address += incDec;
			break;
		}

		// 8 bit register-register load
		case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
		case 0x50: case 0x51: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5A: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6E: case 0x6F:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
		case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E:
		{
			LD_R(instruction);
			break;
		}
		case 0x76:
			mHalted = true;
			break;

		// Arithmetic/Logic
		case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
		case 0x90: case 0x91: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9A: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
		case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA5: case 0xA6: case 0xA7:
		case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAE: case 0xAF:
		case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7:
		case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE:
		{
			ALU_R(instruction);
			break;
		}

		// CB Instruction Prefix
		case 0xCB: 
			HandleCBInstruction(instruction);
		}

		return sCycles[instruction];
	}

	int CPU::HandleCBInstruction(OpCode instruction)
	{
		return 0;
	}

#pragma region OpCodeFunctions
	void CPU::LD_R(OpCode op)
	{
		Byte src;
		READ_REG(op.z(), src);
		WRITE_REG(op.y(), src);
	}

	void CPU::ALU_R(OpCode op)
	{
		Byte target = op.z();
		switch (op.y())
		{
		case 0: ADD_R(target); break;
		case 1: ADC_R(target); break;
		case 2: SUB_R(target); break;
		case 3: SBC_R(target); break;
		case 4: AND_R(target); break;
		case 5: XOR_R(target); break;
		case 6: OR_R(target); break;
		case 7: CP_R(target); break;
		}
	}




	void CPU::ADD_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;
		Byte value;
		READ_REG(target, value);

		Word result = regA + value;
		regA = (Byte)result;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? _HCarryFlag : 0;
		flags |= ((result & 0x100) != 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::ADC_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;
		Byte value;
		READ_REG(target, value);

		Word result = regA + value + (mFRegister.carry() ? 1 : 0);
		regA = (Byte)result;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? _HCarryFlag : 0;
		flags |= ((result & 0x100) != 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::SUB_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;
		Byte value;
		READ_REG(target, value);

		Byte result = regA - value;
		regA = result;

		Flag flags = _SubtractFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) + (value & 0xf) > 0xf) ? _HCarryFlag : 0;
		flags |= ((result & 0x100) != 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::SBC_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte regACpy = regA;
		Byte value;
		READ_REG(target, value);

		Byte carry = mFRegister.carry() ? 1 : 0;
		SWord result = regA - value - carry;
		regA = (Byte)result;

		Flag flags = _SubtractFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regACpy & 0xf) - (value & 0xf) - carry) ? _HCarryFlag : 0;
		flags |= (result < 0) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::AND_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte value;
		READ_REG(target, value);

		regA &= value;

		Flag flags = _HCarryFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::XOR_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte value;
		READ_REG(target, value);

		regA ^= value;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::OR_R(Byte target)
	{
		Byte& regA = mRegisters[ByteReg::A];
		Byte value;
		READ_REG(target, value);

		regA |= value;

		Flag flags = 0;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		mFRegister.setFlags(flags);
	}

	void CPU::CP_R(Byte target)
	{
		Byte regA = mRegisters[ByteReg::A];
		Byte value;
		READ_REG(target, value);

		Byte result = regA - value;

		Flag flags = _SubtractFlag;
		flags |= (regA == 0) ? _ZeroFlag : 0;
		flags |= ((regA & 0xf) - (value & 0xf) < 0) ? _HCarryFlag : 0;
		flags |= (regA < value) ? _CarryFlag : 0;
		mFRegister.setFlags(flags);
	}
#pragma endregion
}

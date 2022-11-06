#pragma once

#include "Register.h"

namespace GB {

	struct OpCode
	{
		Byte data;

		OpCode(Byte _data) : data(_data) {}
		operator Byte() const { return data; }

		Byte x() const { return data >> 6; }
		Byte y() const { return (data >> 3) & 0x07; }
		Byte z() const { return data & 0x07; }

		Byte p() const { return (data >> 4) & 0x03; }
		Byte q() const { return (data >> 3) & 0x01; }

		Byte i() const { return (data >> 5) & 0x01; }
	};

	using RegTarget = std::variant<ByteReg, WordReg>;
	using RegTargets = std::array<RegTarget, 8>;
	using RegPairTargets = std::array<WordReg, 4>;

	GB_CONST RegTargets sRegisters = { ByteReg::B, ByteReg::C, ByteReg::D, ByteReg::E, ByteReg::H, ByteReg::L, WordReg::HL, ByteReg::A };
	GB_CONST RegPairTargets sRegisterPairs = { WordReg::BC, WordReg::DE, WordReg::HL, WordReg::SP };
	GB_CONST RegPairTargets sRegisterPairs2 = { WordReg::BC, WordReg::DE, WordReg::HL, WordReg::AF };

#define READ_REG(target, val)	std::visit([&](auto&& arg)\
								{\
									using T = std::decay_t<decltype(arg)>;\
									if_c(std::is_same_v<T, ByteReg>)\
										val = mRegisters[arg];\
									else if_c(std::is_same_v<T, WordReg>)\
										val = AddressBus::Read(mRegisters[arg]);\
									else\
										GB_SASSERT(false, "Non-exhaustive visitor!");\
								}, sRegisters[target]);

#define WRITE_REG(target, val)	std::visit([&](auto&& arg)\
								{\
									using T = std::decay_t<decltype(arg)>;\
									if_c(std::is_same_v<T, ByteReg>)\
										mRegisters[arg] = val;\
									else if_c(std::is_same_v<T, WordReg>)\
										AddressBus::Write(mRegisters[arg], val);\
									else\
										GB_SASSERT(false, "Non-exhaustive visitor!");\
								}, sRegisters[target]);

#define WRITE_REG_PAIR(target, val)	std::visit([&](auto&& arg)\
									{\
										using T = std::decay_t<decltype(arg)>;\
										if_c(std::is_same_v<T, ByteReg>)\
											mRegisters[arg] = val;\
										else if_c(std::is_same_v<T, WordReg>)\
											\
										else\
											GB_SASSERT(false, "Non-exhaustive visitor!");\
									}, sRegisterPairs[target]);
	GB_CONST int sCycles[256] =
	{
		0
	};

	//GB_CONST Instruction sInstructions[] =
	//{
	//	/*Mnemonic                 CB IMM R8  R8  R16 CC  BIT */
 //       { "NOP",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD r16, imm16",       0, 2,  -1, -1,  4, -1, -1 },
 //       { "LD (BC), A",          0, 0,  -1, -1, -1, -1, -1 },
 //       { "INC r16",             0, 0,  -1, -1,  4, -1, -1 },
 //       { "INC r8",              0, 0,   3, -1, -1, -1, -1 },
 //       { "DEC r8",              0, 0,   3, -1, -1, -1, -1 },
 //       { "LD r8, imm8",         0, 1,   3, -1, -1, -1, -1 },
 //       { "RLCA",                0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD (imm16), SP",      0, 2,  -1, -1, -1, -1, -1 },
 //       { "ADD HL, r16",         0, 0,  -1, -1,  4, -1, -1 },
 //       { "LD A, (BC)",          0, 0,  -1, -1, -1, -1, -1 },
 //       { "DEC r16",             0, 0,  -1, -1,  4, -1, -1 },
 //       { "RRCA",                0, 0,  -1, -1, -1, -1, -1 },
 //       { "STOP",                0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD (DE), A",          0, 0,  -1, -1, -1, -1, -1 },
 //       { "RLA",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "JR off8",             0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD A, (DE)",          0, 0,  -1, -1, -1, -1, -1 },
 //       { "RRA",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "JR cc, off8",         0, 1,  -1, -1, -1,  3, -1 },
 //       { "LD (HL+), A",         0, 0,  -1, -1, -1, -1, -1 },
 //       { "DAA",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD A, (HL+)",         0, 0,  -1, -1, -1, -1, -1 },
 //       { "CPL",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD (HL-), A",         0, 0,  -1, -1, -1, -1, -1 },
 //       { "SCF",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD A, (HL-)",         0, 0,  -1, -1, -1, -1, -1 },
 //       { "CCF",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD r8, r8",           0, 0,   0,  3, -1, -1, -1 }, // Also generates HALT (for LD (HL), (HL) slot)
 //       { "ADD r8",              0, 0,   0, -1, -1, -1, -1 },
 //       { "ADC r8",              0, 0,   0, -1, -1, -1, -1 },
 //       { "SUB r8",              0, 0,   0, -1, -1, -1, -1 },
 //       { "SBC r8",              0, 0,   0, -1, -1, -1, -1 },
 //       { "AND r8",              0, 0,   0, -1, -1, -1, -1 },
 //       { "XOR r8",              0, 0,   0, -1, -1, -1, -1 },
 //       { "OR r8",               0, 0,   0, -1, -1, -1, -1 },
 //       { "CP r8",               0, 0,   0, -1, -1, -1, -1 },
 //       { "RET cc",              0, 0,  -1, -1, -1,  3, -1 },
 //       { "POP r16",             0, 0,  -1, -1,  4, -1, -1 },
 //       { "JP cc, imm16",        0, 2,  -1, -1, -1,  3, -1 },
 //       { "JP imm16",            0, 2,  -1, -1, -1, -1, -1 },
 //       { "CALL cc, imm16",      0, 2,  -1, -1, -1,  3, -1 },
 //       { "PUSH r16",            0, 0,  -1, -1,  4, -1, -1 },
 //       { "RST vec",             0, 0,  -1, -1, -1, -1,  3 },
 //       { "RET",                 0, 0,  -1, -1, -1, -1, -1 },
 //       { "CALL imm16",          0, 2,  -1, -1, -1, -1, -1 },
 //       { "ADD imm8",            0, 1,  -1, -1, -1, -1, -1 },
 //       { "ADC imm8",            0, 1,  -1, -1, -1, -1, -1 },
 //       { "SUB imm8",            0, 1,  -1, -1, -1, -1, -1 },
 //       { "RETI",                0, 0,  -1, -1, -1, -1, -1 },
 //       { "SBC imm8",            0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD ($ff00+imm8), A",  0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD ($ff00+C), A",     0, 0,  -1, -1, -1, -1, -1 },
 //       { "AND imm8",            0, 1,  -1, -1, -1, -1, -1 },
 //       { "ADD SP, imm8",        0, 1,  -1, -1, -1, -1, -1 },
 //       { "JP HL",               0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD (imm16), A",       0, 2,  -1, -1, -1, -1, -1 },
 //       { "XOR imm8",            0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD A, ($ff00+imm8)",  0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD A, ($ff00+C)",     0, 0,  -1, -1, -1, -1, -1 },
 //       { "DI",                  0, 0,  -1, -1, -1, -1, -1 },
 //       { "OR imm8",             0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD HL, SP + imm8",    0, 1,  -1, -1, -1, -1, -1 },
 //       { "LD SP, HL",           0, 0,  -1, -1, -1, -1, -1 },
 //       { "LD A, (imm16)",       0, 2,  -1, -1, -1, -1, -1 },
 //       { "EI",                  0, 0,  -1, -1, -1, -1, -1 },
 //       { "CP imm8",             0, 1,  -1, -1, -1, -1, -1 },
	//};

}
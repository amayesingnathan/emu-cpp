#pragma once

#include "Flags.h"

namespace GB {

	using OpCode = Byte;

	struct Instruction
	{
	};

	//GB_CONST Instruction sInstructions[] =
	//{
	//	/*Mnemonic               CB IMM R8  R8  R16 CC  BIT */
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
#include "gbpch.h"
#include "CPU.h"

namespace GB {

	int CPU::HandleInstruction(OpCode instruction)
	{

	}

	void CPU::_OpcodeADC(Byte value)
	{
	}
	void CPU::OpcodeADC()
	{
	}
	void CPU::OpcodeADC(ByteReg reg)
	{
	}
	void CPU::OpcodeADC(Word addr)
	{
	}
	void CPU::_OpcodeADD(Byte reg, Byte value)
	{
	}
	void CPU::OpcodeADD_A()
	{
	}
	void CPU::OpcodeADD_A(ByteReg reg)
	{
	}
	void CPU::OpcodeADD_A(Word addr)
	{
	}
	void CPU::_OpcodeADD_HL(Word value)
	{
	}
	void CPU::OpcodeADD_HL(Register regPair)
	{
	}
	void CPU::OpcodeADD_HL(WordReg wordReg)
	{
	}
	void CPU::OpcodeADD_SP()
	{
	}
	void CPU::OpcodeADD_SIGNED()
	{
	}
	void CPU::_OpcodeAND(Byte value)
	{
	}
	void CPU::OpcodeAND()
	{
	}
	void CPU::OpcodeAND(ByteReg reg)
	{
	}
	void CPU::OpcodeAND(Word addr)
	{
	}
	void CPU::_OpcodeBIT(Byte bit, Byte value)
	{
	}
	void CPU::OpcodeBIT(Byte bit, ByteReg reg)
	{
	}
	void CPU::OpcodeBIT(Byte bit, Word addr)
	{
	}
	void CPU::OpcodeCALL()
	{
	}
	void CPU::OpcodeCALL(Condition condition)
	{
	}
	void CPU::OpcodeCCF()
	{
	}
	void CPU::_OpcodeCP(Byte value)
	{
	}
	void CPU::OpcodeCP()
	{
	}
	void CPU::OpcodeCP(ByteReg reg)
	{
	}
	void CPU::OpcodeCP(Word addr)
	{
	}
	void CPU::OpcodeCPL()
	{
	}
	void CPU::OpcodeDAA()
	{
	}
	void CPU::OpcodeDEC(ByteReg reg)
	{
	}
	void CPU::OpcodeDEC(Register reg)
	{
	}
	void CPU::OpcodeDEC(WordReg reg)
	{
	}
	void CPU::OpcodeDEC(Word addr)
	{
	}
	void CPU::OpcodeDI()
	{
	}
	void CPU::OpcodeEI()
	{
	}
	void CPU::OpcodeINC(ByteReg reg)
	{
	}
	void CPU::OpcodeINC(Register reg)
	{
	}
	void CPU::OpcodeINC(WordReg reg)
	{
	}
	void CPU::OpcodeINC(Word addr)
	{
	}
	void CPU::OpcodeJP()
	{
	}
	void CPU::OpcodeJP(Condition condition)
	{
	}
	void CPU::OpcodeJP(Word addr)
	{
	}
	void CPU::OpcodeJR()
	{
	}
	void CPU::OpcodeJR(Condition condition)
	{
	}
	void CPU::OpcodeHALT()
	{
	}
	void CPU::OpcodeLD(ByteReg reg)
	{
	}
	void CPU::OpcodeLD(ByteReg reg, ByteReg byteReg)
	{
	}
	void CPU::OpcodeLD(ByteReg reg, Word address)
	{
	}
	void CPU::OpcodeLD(Register reg)
	{
	}
	void CPU::OpcodeLD(WordReg reg)
	{
	}
	void CPU::OpcodeLD(WordReg reg, Register regPair)
	{
	}
	void CPU::OpcodeLD(Word address)
	{
	}
	void CPU::OpcodeLD(Word address, ByteReg byte_reg)
	{
	}
	void CPU::OpcodeLD(Word address, WordReg word_reg)
	{
	}
	void CPU::OpcodeLD_TO_ADDR(ByteReg reg)
	{
	}
	void CPU::OpcodeLD_FROM_ADDR(ByteReg reg)
	{
	}
	Byte CPU::_OpcodeLDD(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeLDD(ByteReg reg, Word address)
	{
	}
	void CPU::OpcodeLDD(Word address, ByteReg reg)
	{
	}
	void CPU::OpcodeLDH_INTO_A()
	{
	}
	void CPU::OpcodeLDH_INTO_DATA()
	{
	}
	void CPU::OpcodeLDH_INTO_C()
	{
	}
	void CPU::OpcodeLDH_C_INTO_A()
	{
	}
	void CPU::OpcodeLDHL()
	{
	}
	void CPU::OpcodeLDI(ByteReg reg, Word address)
	{
	}
	void CPU::OpcodeLDI(Word address, ByteReg reg)
	{
	}
	void CPU::OpcodeNOP()
	{
	}
	void CPU::_OpcodeOR(Byte value)
	{
	}
	void CPU::OpcodeOR()
	{
	}
	void CPU::OpcodeOR(ByteReg reg)
	{
	}
	void CPU::OpcodeOR(Word addr)
	{
	}
	void CPU::OpcodePOP(Register reg)
	{
	}
	void CPU::OpcodePUSH(Register reg)
	{
	}
	void CPU::OpcodeRES(Byte bit, ByteReg reg)
	{
	}
	void CPU::OpcodeRES(Byte bit, Word addr)
	{
	}
	void CPU::OpcodeRET()
	{
	}
	void CPU::OpcodeRET(Condition condition)
	{
	}
	void CPU::OpcodeRETI()
	{
	}
	Byte CPU::_OpcodeRL(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeRLA()
	{
	}
	void CPU::OpcodeRL(ByteReg reg)
	{
	}
	void CPU::OpcodeRL(Word addr)
	{
	}
	Byte CPU::_OpcodeRLC(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeRLCA()
	{
	}
	void CPU::OpcodeRLC(ByteReg reg)
	{
	}
	void CPU::OpcodeRLC(Word addr)
	{
	}
	Byte CPU::_OpcodeRR(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeRRA()
	{
	}
	void CPU::OpcodeRR(ByteReg reg)
	{
	}
	void CPU::OpcodeRR(Word addr)
	{
	}
	Byte CPU::_OpcodeRRC(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeRRCA()
	{
	}
	void CPU::OpcodeRRC(ByteReg reg)
	{
	}
	void CPU::OpcodeRRC(Word addr)
	{
	}
	void CPU::OpcodeRST(Byte offset)
	{
	}
	void CPU::_OpcodeSBC(Byte value)
	{
	}
	void CPU::OpcodeSBC()
	{
	}
	void CPU::OpcodeSBC(ByteReg reg)
	{
	}
	void CPU::OpcodeSBC(Word addr)
	{
	}
	void CPU::OpcodeSCF()
	{
	}
	void CPU::OpcodeSET(Byte bit, ByteReg reg)
	{
	}
	void CPU::OpcodeSET(Byte bit, Word addr)
	{
	}
	Byte CPU::_OpcodeSLA(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeSLA(ByteReg reg)
	{
	}
	void CPU::OpcodeSLA(Word addr)
	{
	}
	Byte CPU::_OpcodeSRA(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeSRA(ByteReg reg)
	{
	}
	void CPU::OpcodeSRA(Word addr)
	{
	}
	Byte CPU::_OpcodeSRL(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeSRL(ByteReg reg)
	{
	}
	void CPU::OpcodeSRL(Word addr)
	{
	}
	void CPU::OpcodeSTOP()
	{
	}
	void CPU::_OpcodeSUB(Byte value)
	{
	}
	void CPU::OpcodeSUB()
	{
	}
	void CPU::OpcodeSUB(ByteReg reg)
	{
	}
	void CPU::OpcodeSUB(Word addr)
	{
	}
	Byte CPU::_OpcodeSWAP(Byte value)
	{
		return Byte();
	}
	void CPU::OpcodeSWAP(ByteReg reg)
	{
	}
	void CPU::OpcodeSWAP(Word addr)
	{
	}
	void CPU::_OpcodeXOR(Byte value)
	{
	}
	void CPU::OpcodeXOR()
	{
	}
	void CPU::OpcodeXOR(ByteReg reg)
	{
	}
	void CPU::OpcodeXOR(Word addr)
	{
	}



	void CPU::Opcode00()
	{
		// NOP
	}
	void CPU::Opcode01()
	{
	}
	void CPU::Opcode02()
	{
	}
	void CPU::Opcode03()
	{
	}
	void CPU::Opcode04()
	{
	}
	void CPU::Opcode05()
	{
	}
	void CPU::Opcode06()
	{
	}
	void CPU::Opcode07()
	{
	}
	void CPU::Opcode08()
	{
	}
	void CPU::Opcode09()
	{
	}
	void CPU::Opcode0A()
	{
	}
	void CPU::Opcode0B()
	{
	}
	void CPU::Opcode0C()
	{
	}
	void CPU::Opcode0D()
	{
	}
	void CPU::Opcode0E()
	{
	}
	void CPU::Opcode0F()
	{
	}
	void CPU::Opcode10()
	{
	}
	void CPU::Opcode11()
	{
	}
	void CPU::Opcode12()
	{
	}
	void CPU::Opcode13()
	{
	}
	void CPU::Opcode14()
	{
	}
	void CPU::Opcode15()
	{
	}
	void CPU::Opcode16()
	{
	}
	void CPU::Opcode17()
	{
	}
	void CPU::Opcode18()
	{
	}
	void CPU::Opcode19()
	{
	}
	void CPU::Opcode1A()
	{
	}
	void CPU::Opcode1B()
	{
	}
	void CPU::Opcode1C()
	{
	}
	void CPU::Opcode1D()
	{
	}
	void CPU::Opcode1E()
	{
	}
	void CPU::Opcode1F()
	{
	}
	void CPU::Opcode20()
	{
	}
	void CPU::Opcode21()
	{
	}
	void CPU::Opcode22()
	{
	}
	void CPU::Opcode23()
	{
	}
	void CPU::Opcode24()
	{
	}
	void CPU::Opcode25()
	{
	}
	void CPU::Opcode26()
	{
	}
	void CPU::Opcode27()
	{
	}
	void CPU::Opcode28()
	{
	}
	void CPU::Opcode29()
	{
	}
	void CPU::Opcode2A()
	{
	}
	void CPU::Opcode2B()
	{
	}
	void CPU::Opcode2C()
	{
	}
	void CPU::Opcode2D()
	{
	}
	void CPU::Opcode2E()
	{
	}
	void CPU::Opcode2F()
	{
	}
	void CPU::Opcode30()
	{
	}
	void CPU::Opcode31()
	{
	}
	void CPU::Opcode32()
	{
	}
	void CPU::Opcode33()
	{
	}
	void CPU::Opcode34()
	{
	}
	void CPU::Opcode35()
	{
	}
	void CPU::Opcode36()
	{
	}
	void CPU::Opcode37()
	{
	}
	void CPU::Opcode38()
	{
	}
	void CPU::Opcode39()
	{
	}
	void CPU::Opcode3A()
	{
	}
	void CPU::Opcode3B()
	{
	}
	void CPU::Opcode3C()
	{
	}
	void CPU::Opcode3D()
	{
	}
	void CPU::Opcode3E()
	{
	}
	void CPU::Opcode3F()
	{
	}
	void CPU::Opcode40()
	{
	}
	void CPU::Opcode41()
	{
	}
	void CPU::Opcode42()
	{
	}
	void CPU::Opcode43()
	{
	}
	void CPU::Opcode44()
	{
	}
	void CPU::Opcode45()
	{
	}
	void CPU::Opcode46()
	{
	}
	void CPU::Opcode47()
	{
	}
	void CPU::Opcode48()
	{
	}
	void CPU::Opcode49()
	{
	}
	void CPU::Opcode4A()
	{
	}
	void CPU::Opcode4B()
	{
	}
	void CPU::Opcode4C()
	{
	}
	void CPU::Opcode4D()
	{
	}
	void CPU::Opcode4E()
	{
	}
	void CPU::Opcode4F()
	{
	}
	void CPU::Opcode50()
	{
	}
	void CPU::Opcode51()
	{
	}
	void CPU::Opcode52()
	{
	}
	void CPU::Opcode53()
	{
	}
	void CPU::Opcode54()
	{
	}
	void CPU::Opcode55()
	{
	}
	void CPU::Opcode56()
	{
	}
	void CPU::Opcode57()
	{
	}
	void CPU::Opcode58()
	{
	}
	void CPU::Opcode59()
	{
	}
	void CPU::Opcode5A()
	{
	}
	void CPU::Opcode5B()
	{
	}
	void CPU::Opcode5C()
	{
	}
	void CPU::Opcode5D()
	{
	}
	void CPU::Opcode5E()
	{
	}
	void CPU::Opcode5F()
	{
	}
	void CPU::Opcode60()
	{
	}
	void CPU::Opcode61()
	{
	}
	void CPU::Opcode62()
	{
	}
	void CPU::Opcode63()
	{
	}
	void CPU::Opcode64()
	{
	}
	void CPU::Opcode65()
	{
	}
	void CPU::Opcode66()
	{
	}
	void CPU::Opcode67()
	{
	}
	void CPU::Opcode68()
	{
	}
	void CPU::Opcode69()
	{
	}
	void CPU::Opcode6A()
	{
	}
	void CPU::Opcode6B()
	{
	}
	void CPU::Opcode6C()
	{
	}
	void CPU::Opcode6D()
	{
	}
	void CPU::Opcode6E()
	{
	}
	void CPU::Opcode6F()
	{
	}
	void CPU::Opcode70()
	{
	}
	void CPU::Opcode71()
	{
	}
	void CPU::Opcode72()
	{
	}
	void CPU::Opcode73()
	{
	}
	void CPU::Opcode74()
	{
	}
	void CPU::Opcode75()
	{
	}
	void CPU::Opcode76()
	{
	}
	void CPU::Opcode77()
	{
	}
	void CPU::Opcode78()
	{
	}
	void CPU::Opcode79()
	{
	}
	void CPU::Opcode7A()
	{
	}
	void CPU::Opcode7B()
	{
	}
	void CPU::Opcode7C()
	{
	}
	void CPU::Opcode7D()
	{
	}
	void CPU::Opcode7E()
	{
	}
	void CPU::Opcode7F()
	{
	}
	void CPU::Opcode80()
	{
	}
	void CPU::Opcode81()
	{
	}
	void CPU::Opcode82()
	{
	}
	void CPU::Opcode83()
	{
	}
	void CPU::Opcode84()
	{
	}
	void CPU::Opcode85()
	{
	}
	void CPU::Opcode86()
	{
	}
	void CPU::Opcode87()
	{
	}
	void CPU::Opcode88()
	{
	}
	void CPU::Opcode89()
	{
	}
	void CPU::Opcode8A()
	{
	}
	void CPU::Opcode8B()
	{
	}
	void CPU::Opcode8C()
	{
	}
	void CPU::Opcode8D()
	{
	}
	void CPU::Opcode8E()
	{
	}
	void CPU::Opcode8F()
	{
	}
	void CPU::Opcode90()
	{
	}
	void CPU::Opcode91()
	{
	}
	void CPU::Opcode92()
	{
	}
	void CPU::Opcode93()
	{
	}
	void CPU::Opcode94()
	{
	}
	void CPU::Opcode95()
	{
	}
	void CPU::Opcode96()
	{
	}
	void CPU::Opcode97()
	{
	}
	void CPU::Opcode98()
	{
	}
	void CPU::Opcode99()
	{
	}
	void CPU::Opcode9A()
	{
	}
	void CPU::Opcode9B()
	{
	}
	void CPU::Opcode9C()
	{
	}
	void CPU::Opcode9D()
	{
	}
	void CPU::Opcode9E()
	{
	}
	void CPU::Opcode9F()
	{
	}
	void CPU::OpcodeA0()
	{
	}
	void CPU::OpcodeA1()
	{
	}
	void CPU::OpcodeA2()
	{
	}
	void CPU::OpcodeA3()
	{
	}
	void CPU::OpcodeA4()
	{
	}
	void CPU::OpcodeA5()
	{
	}
	void CPU::OpcodeA6()
	{
	}
	void CPU::OpcodeA7()
	{
	}
	void CPU::OpcodeA8()
	{
	}
	void CPU::OpcodeA9()
	{
	}
	void CPU::OpcodeAA()
	{
	}
	void CPU::OpcodeAB()
	{
	}
	void CPU::OpcodeAC()
	{
	}
	void CPU::OpcodeAD()
	{
	}
	void CPU::OpcodeAE()
	{
	}
	void CPU::OpcodeAF()
	{
	}
	void CPU::OpcodeB0()
	{
	}
	void CPU::OpcodeB1()
	{
	}
	void CPU::OpcodeB2()
	{
	}
	void CPU::OpcodeB3()
	{
	}
	void CPU::OpcodeB4()
	{
	}
	void CPU::OpcodeB5()
	{
	}
	void CPU::OpcodeB6()
	{
	}
	void CPU::OpcodeB7()
	{
	}
	void CPU::OpcodeB8()
	{
	}
	void CPU::OpcodeB9()
	{
	}
	void CPU::OpcodeBA()
	{
	}
	void CPU::OpcodeBB()
	{
	}
	void CPU::OpcodeBC()
	{
	}
	void CPU::OpcodeBD()
	{
	}
	void CPU::OpcodeBE()
	{
	}
	void CPU::OpcodeBF()
	{
	}
	void CPU::OpcodeC0()
	{
	}
	void CPU::OpcodeC1()
	{
	}
	void CPU::OpcodeC2()
	{
	}
	void CPU::OpcodeC3()
	{
	}
	void CPU::OpcodeC4()
	{
	}
	void CPU::OpcodeC5()
	{
	}
	void CPU::OpcodeC6()
	{
	}
	void CPU::OpcodeC7()
	{
	}
	void CPU::OpcodeC8()
	{
	}
	void CPU::OpcodeC9()
	{
	}
	void CPU::OpcodeCA()
	{
	}
	void CPU::OpcodeCB()
	{
	}
	void CPU::OpcodeCC()
	{
	}
	void CPU::OpcodeCD()
	{
	}
	void CPU::OpcodeCE()
	{
	}
	void CPU::OpcodeCF()
	{
	}
	void CPU::OpcodeD0()
	{
	}
	void CPU::OpcodeD1()
	{
	}
	void CPU::OpcodeD2()
	{
	}
	void CPU::OpcodeD3()
	{
	}
	void CPU::OpcodeD4()
	{
	}
	void CPU::OpcodeD5()
	{
	}
	void CPU::OpcodeD6()
	{
	}
	void CPU::OpcodeD7()
	{
	}
	void CPU::OpcodeD8()
	{
	}
	void CPU::OpcodeD9()
	{
	}
	void CPU::OpcodeDA()
	{
	}
	void CPU::OpcodeDB()
	{
	}
	void CPU::OpcodeDC()
	{
	}
	void CPU::OpcodeDD()
	{
	}
	void CPU::OpcodeDE()
	{
	}
	void CPU::OpcodeDF()
	{
	}
	void CPU::OpcodeE0()
	{
	}
	void CPU::OpcodeE1()
	{
	}
	void CPU::OpcodeE2()
	{
	}
	void CPU::OpcodeE3()
	{
	}
	void CPU::OpcodeE4()
	{
	}
	void CPU::OpcodeE5()
	{
	}
	void CPU::OpcodeE6()
	{
	}
	void CPU::OpcodeE7()
	{
	}
	void CPU::OpcodeE8()
	{
	}
	void CPU::OpcodeE9()
	{
	}
	void CPU::OpcodeEA()
	{
	}
	void CPU::OpcodeEB()
	{
	}
	void CPU::OpcodeEC()
	{
	}
	void CPU::OpcodeED()
	{
	}
	void CPU::OpcodeEE()
	{
	}
	void CPU::OpcodeEF()
	{
	}
	void CPU::OpcodeF0()
	{
	}
	void CPU::OpcodeF1()
	{
	}
	void CPU::OpcodeF2()
	{
	}
	void CPU::OpcodeF3()
	{
	}
	void CPU::OpcodeF4()
	{
	}
	void CPU::OpcodeF5()
	{
	}
	void CPU::OpcodeF6()
	{
	}
	void CPU::OpcodeF7()
	{
	}
	void CPU::OpcodeF8()
	{
	}
	void CPU::OpcodeF9()
	{
	}
	void CPU::OpcodeFA()
	{
	}
	void CPU::OpcodeFB()
	{
	}
	void CPU::OpcodeFC()
	{
	}
	void CPU::OpcodeFD()
	{
	}
	void CPU::OpcodeFE()
	{
	}
	void CPU::OpcodeFF()
	{
	}
}
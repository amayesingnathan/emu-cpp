#pragma once

#include "Register.h"
#include "Flags.h"
#include "Instructions.h"

namespace GB {

    enum class Condition {
        NZ, Z, NC, C,
    };

	class CPU
	{
	public:
		CPU()
			: mFlagsRegister(mRegisters[ByteReg::F])
		{

		}

    private:
        bool IsCondition(Condition condition)
        {
            bool shouldBranch;

            switch (condition) {
            case Condition::C:
                shouldBranch = mFlagsRegister.carry();
                break;
            case Condition::NC:
                shouldBranch = !mFlagsRegister.carry();
                break;
            case Condition::Z:
                shouldBranch = mFlagsRegister.zero();
                break;
            case Condition::NZ:
                shouldBranch = !mFlagsRegister.zero();
                break;
            }

            /* If the branch is taken, remember so that the correct processor cycles
             * can be used */
            mBranchTaken = shouldBranch;
            return shouldBranch;
        }

	private:
		Registers mRegisters;
		Register mProgramCounter = 0x100;
		Register mStackPointer = 0xFFFE;

		FlagsRegister mFlagsRegister;

        bool mInteruptsEnabled = false;
        bool mHalted = false;

        bool mBranchTaken = false;

#pragma region OpCodeFunctions
	private:
		void HandleInstruction(OpCode instruction);

        /* ADC */
        inline void _OpcodeADC(Byte value);
        
        inline void OpcodeADC();
        inline void OpcodeADC(ByteReg reg);
        inline void OpcodeADC(Word addr);
        
        /* ADD */
        inline void _OpcodeADD(Byte reg, Byte value);
        
        inline void OpcodeADD_A();
        inline void OpcodeADD_A(ByteReg reg);
        inline void OpcodeADD_A(Word addr);
        
        inline void _OpcodeADD_HL(Word value);
        inline void OpcodeADD_HL(Register regPair);
        inline void OpcodeADD_HL(WordReg wordReg);
        
        inline void OpcodeADD_SP();
        
        inline void OpcodeADD_SIGNED();
        
        /* AND */
        inline void _OpcodeAND(Byte value);
        
        inline void OpcodeAND();
        inline void OpcodeAND(ByteReg reg);
        inline void OpcodeAND(Word addr);
        
        /* BIT */
        inline void _OpcodeBIT(Byte bit, Byte value);
        
        inline void OpcodeBIT(Byte bit, ByteReg reg);
        inline void OpcodeBIT(Byte bit, Word addr);
        
        /* CALL */
        inline void OpcodeCall();
        inline void OpcodeCall(Condition condition);
        
        /* CCF */
        inline void OpcodeCCF();
        
        /* CP */
        inline void _OpcodeCP(Byte value);
        
        inline void OpcodeCP();
        inline void OpcodeCP(ByteReg reg);
        inline void OpcodeCP(Word addr);
        
        /* CPL */
        inline void OpcodeCPL();
        
        /* DAA */
        inline void OpcodeDAA();
        
        inline /* DEC */
        inline void OpcodeDEC(ByteReg reg);
        inline void OpcodeDEC(Register reg);
        inline void OpcodeDEC(WordReg reg);
        inline void OpcodeDEC(Word addr);
        
        /* DI */
        inline void OpcodeDI();
        
        /* EI */
        inline void OpcodeEI();
        
        /* INC */
        inline void OpcodeINC(ByteReg reg);
        inline void OpcodeINC(Register reg);
        inline void OpcodeINC(WordReg reg);
        inline void OpcodeINC(Word addr);
        
        /* JP */
        inline void OpcodeJP();
        inline void OpcodeJP(Condition condition);
        inline void OpcodeJP(Word addr);
        
        /* JR */
        inline void OpcodeJR();
        inline void OpcodeJR(Condition condition);
        
        /* HALT */
        inline void OpcodeHALT();
        
        /* LD */
        inline void OpcodeLD(ByteReg reg);
        inline void OpcodeLD(ByteReg reg, ByteReg byteReg);
        inline void OpcodeLD(ByteReg reg, Word address);
        
        inline void OpcodeLD(Register reg);
        
        inline void OpcodeLD(WordReg& reg);
        inline void OpcodeLD(WordReg& reg, Register regPair);
        
        inline void OpcodeLD(Word address);
        inline void OpcodeLD(Word address, ByteReg byte_reg);
        inline void OpcodeLD(Word address, WordReg word_reg);
        
        // (nn), A
        inline void OpcodeLD_TO_ADDR(ByteReg reg);
        inline void OpcodeLD_FROM_ADDR(ByteReg reg);
        
        /* LDD */
        inline Byte _OpcodeLDD(Byte value);
        
        inline void OpcodeLDD(ByteReg reg, Word address);
        inline void OpcodeLDD(Word address, ByteReg reg);
        
        /* LDH */
        // A, (n)
        inline void OpcodeLDH_INTO_A();
        // (n), A
        inline void OpcodeLDH_INTO_DATA();
        // (reg), A
        inline void OpcodeLDH_INTO_C();
        // A, (reg)
        inline void OpcodeLDH_C_INTO_A();
        
        /* LDHL */
        inline void OpcodeLDHL();
        
        /* LDI */
        inline void OpcodeLDI(ByteReg reg, Word address);
        inline void OpcodeLDI(Word address, ByteReg reg);
        
        /* NOP */
        inline void OpcodeNOP();
        
        /* OR */
        inline void _OpcodeOR(Byte value);
        
        inline void OpcodeOR();
        inline void OpcodeOR(ByteReg reg);
        inline void OpcodeOR(Word addr);
        
        /* POP */
        inline void OpcodePOP(Register reg);
        
        /* PUSH */
        inline void OpcodePUSH(Register reg);
        
        /* RES */
        inline void OpcodeRES(Byte bit, ByteReg reg);
        inline void OpcodeRES(Byte bit, Word addr);
        
        /* RET */
        inline void OpcodeRET();
        inline void OpcodeRET(Condition condition);
        
        /* RETI */
        inline void OpcodeRETI();
        
        /* RL */
        inline Byte _OpcodeRL(Byte value);
        
        inline void OpcodeRLA();
        inline void OpcodeRL(ByteReg reg);
        inline void OpcodeRL(Word addr);
        inline 
        /* RLC */
        inline Byte _OpcodeRLC(Byte value);
        
        inline void OpcodeRLCA();
        inline void OpcodeRLC(ByteReg reg);
        inline void OpcodeRLC(Word addr);
        
        /* RR */
        inline Byte _OpcodeRR(Byte value);
        
        inline void OpcodeRRA();
        inline void OpcodeRR(ByteReg reg);
        inline void OpcodeRR(Word addr);
        
        /* RRC */
        inline Byte _OpcodeRRC(Byte value);
        
        inline void OpcodeRRCA();
        inline void OpcodeRRC(ByteReg reg);
        inline void OpcodeRRC(Word addr);
        
        /* RST */
        inline void OpcodeRST(Byte offset);
        
        /* SBC */
        inline void _OpcodeSBC(Byte value);
        
        inline void OpcodeSBC();
        inline void OpcodeSBC(ByteReg reg);
        inline void OpcodeSBC(Word addr);
        
        /* SCF */
        inline void OpcodeSCF();
        
        /* SET */
        inline void OpcodeSET(Byte bit, ByteReg reg);
        inline void OpcodeSET(Byte bit, Word addr);
        
        /* SLA */
        inline Byte _OpcodeSLA(Byte value);
        
        inline void OpcodeSLA(ByteReg reg);
        inline void OpcodeSLA(Word addr);
        
        /* SRA */
        inline Byte _OpcodeSRA(Byte value);
        
        inline void OpcodeSRA(ByteReg reg);
        inline void OpcodeSRA(Word addr);
        
        /* SRL */
        inline Byte _OpcodeSRL(Byte value);
        
        inline void OpcodeSRL(ByteReg reg);
        inline void OpcodeSRL(Word addr);
        
        /* STOP */
        inline void OpcodeSTOP();
        
        /* SUB */
        inline void _OpcodeSUB(Byte value);
        
        inline void OpcodeSUB();
        inline void OpcodeSUB(ByteReg reg);
        inline void OpcodeSUB(Word addr);
        
        /* SWAP */
        inline Byte _OpcodeSWAP(Byte value);
        
        inline void OpcodeSWAP(ByteReg reg);
        inline void OpcodeSWAP(Word addr);
        
        /* XOR */
        inline void _OpcodeXOR(Byte value);
        
        inline void OpcodeXOR();
        inline void OpcodeXOR(ByteReg reg);
        inline void OpcodeXOR(Word addr);

		inline void Opcode00(); inline void Opcode01(); inline void Opcode02(); inline void Opcode03(); inline void Opcode04(); inline void Opcode05(); inline void Opcode06(); inline void Opcode07(); 
		inline void Opcode08(); inline void Opcode09(); inline void Opcode0A(); inline void Opcode0B(); inline void Opcode0C(); inline void Opcode0D(); inline void Opcode0E(); inline void Opcode0F();
		inline void Opcode10(); inline void Opcode11(); inline void Opcode12(); inline void Opcode13(); inline void Opcode14(); inline void Opcode15(); inline void Opcode16(); inline void Opcode17();
		inline void Opcode18(); inline void Opcode19(); inline void Opcode1A(); inline void Opcode1B(); inline void Opcode1C(); inline void Opcode1D(); inline void Opcode1E(); inline void Opcode1F();
		inline void Opcode20(); inline void Opcode21(); inline void Opcode22(); inline void Opcode23(); inline void Opcode24(); inline void Opcode25(); inline void Opcode26(); inline void Opcode27();
		inline void Opcode28(); inline void Opcode29(); inline void Opcode2A(); inline void Opcode2B(); inline void Opcode2C(); inline void Opcode2D(); inline void Opcode2E(); inline void Opcode2F();
		inline void Opcode30(); inline void Opcode31(); inline void Opcode32(); inline void Opcode33(); inline void Opcode34(); inline void Opcode35(); inline void Opcode36(); inline void Opcode37();
		inline void Opcode38(); inline void Opcode39(); inline void Opcode3A(); inline void Opcode3B(); inline void Opcode3C(); inline void Opcode3D(); inline void Opcode3E(); inline void Opcode3F();
		inline void Opcode40(); inline void Opcode41(); inline void Opcode42(); inline void Opcode43(); inline void Opcode44(); inline void Opcode45(); inline void Opcode46(); inline void Opcode47();
		inline void Opcode48(); inline void Opcode49(); inline void Opcode4A(); inline void Opcode4B(); inline void Opcode4C(); inline void Opcode4D(); inline void Opcode4E(); inline void Opcode4F();
		inline void Opcode50(); inline void Opcode51(); inline void Opcode52(); inline void Opcode53(); inline void Opcode54(); inline void Opcode55(); inline void Opcode56(); inline void Opcode57();
		inline void Opcode58(); inline void Opcode59(); inline void Opcode5A(); inline void Opcode5B(); inline void Opcode5C(); inline void Opcode5D(); inline void Opcode5E(); inline void Opcode5F();
		inline void Opcode60(); inline void Opcode61(); inline void Opcode62(); inline void Opcode63(); inline void Opcode64(); inline void Opcode65(); inline void Opcode66(); inline void Opcode67();
		inline void Opcode68(); inline void Opcode69(); inline void Opcode6A(); inline void Opcode6B(); inline void Opcode6C(); inline void Opcode6D(); inline void Opcode6E(); inline void Opcode6F();
		inline void Opcode70(); inline void Opcode71(); inline void Opcode72(); inline void Opcode73(); inline void Opcode74(); inline void Opcode75(); inline void Opcode76(); inline void Opcode77();
		inline void Opcode78(); inline void Opcode79(); inline void Opcode7A(); inline void Opcode7B(); inline void Opcode7C(); inline void Opcode7D(); inline void Opcode7E(); inline void Opcode7F();
		inline void Opcode80(); inline void Opcode81(); inline void Opcode82(); inline void Opcode83(); inline void Opcode84(); inline void Opcode85(); inline void Opcode86(); inline void Opcode87();
		inline void Opcode88(); inline void Opcode89(); inline void Opcode8A(); inline void Opcode8B(); inline void Opcode8C(); inline void Opcode8D(); inline void Opcode8E(); inline void Opcode8F();
		inline void Opcode90(); inline void Opcode91(); inline void Opcode92(); inline void Opcode93(); inline void Opcode94(); inline void Opcode95(); inline void Opcode96(); inline void Opcode97();
		inline void Opcode98(); inline void Opcode99(); inline void Opcode9A(); inline void Opcode9B(); inline void Opcode9C(); inline void Opcode9D(); inline void Opcode9E(); inline void Opcode9F();
		inline void OpcodeA0(); inline void OpcodeA1(); inline void OpcodeA2(); inline void OpcodeA3(); inline void OpcodeA4(); inline void OpcodeA5(); inline void OpcodeA6(); inline void OpcodeA7();
		inline void OpcodeA8(); inline void OpcodeA9(); inline void OpcodeAA(); inline void OpcodeAB(); inline void OpcodeAC(); inline void OpcodeAD(); inline void OpcodeAE(); inline void OpcodeAF();
		inline void OpcodeB0(); inline void OpcodeB1(); inline void OpcodeB2(); inline void OpcodeB3(); inline void OpcodeB4(); inline void OpcodeB5(); inline void OpcodeB6(); inline void OpcodeB7();
		inline void OpcodeB8(); inline void OpcodeB9(); inline void OpcodeBA(); inline void OpcodeBB(); inline void OpcodeBC(); inline void OpcodeBD(); inline void OpcodeBE(); inline void OpcodeBF();
		inline void OpcodeC0(); inline void OpcodeC1(); inline void OpcodeC2(); inline void OpcodeC3(); inline void OpcodeC4(); inline void OpcodeC5(); inline void OpcodeC6(); inline void OpcodeC7();
		inline void OpcodeC8(); inline void OpcodeC9(); inline void OpcodeCA(); inline void OpcodeCB(); inline void OpcodeCC(); inline void OpcodeCD(); inline void OpcodeCE(); inline void OpcodeCF();
		inline void OpcodeD0(); inline void OpcodeD1(); inline void OpcodeD2(); inline void OpcodeD3(); inline void OpcodeD4(); inline void OpcodeD5(); inline void OpcodeD6(); inline void OpcodeD7();
		inline void OpcodeD8(); inline void OpcodeD9(); inline void OpcodeDA(); inline void OpcodeDB(); inline void OpcodeDC(); inline void OpcodeDD(); inline void OpcodeDE(); inline void OpcodeDF();
		inline void OpcodeE0(); inline void OpcodeE1(); inline void OpcodeE2(); inline void OpcodeE3(); inline void OpcodeE4(); inline void OpcodeE5(); inline void OpcodeE6(); inline void OpcodeE7();
		inline void OpcodeE8(); inline void OpcodeE9(); inline void OpcodeEA(); inline void OpcodeEB(); inline void OpcodeEC(); inline void OpcodeED(); inline void OpcodeEE(); inline void OpcodeEF();
		inline void OpcodeF0(); inline void OpcodeF1(); inline void OpcodeF2(); inline void OpcodeF3(); inline void OpcodeF4(); inline void OpcodeF5(); inline void OpcodeF6(); inline void OpcodeF7();
		inline void OpcodeF8(); inline void OpcodeF9(); inline void OpcodeFA(); inline void OpcodeFB(); inline void OpcodeFC(); inline void OpcodeFD(); inline void OpcodeFE(); inline void OpcodeFF();
#pragma endregion
	};
}
#pragma once

#include "Memory/AddressBus.h"

#include "Register.h"
#include "Flags.h"
#include "Instructions.h"
#include "WRAM.h"

namespace GB {

    enum class Condition {
        NZ, Z, NC, C,
    };

	class CPU
	{
	public:
		CPU()
			: mFRegister(mRegisters[ByteReg::F])
		{

		}

    public:
        int exec()
        {
            if (mHalted)
                return 0;

            OpCode nextOp = AddressBus::Read(mRegisters[WordReg::PC]++);
            return HandleInstruction(nextOp);
        }

    private:
        bool IsCondition(Condition condition)
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

	private:
		Registers mRegisters;
        FRegister mFRegister;

        MainMemory mMainMemory;

        bool mInteruptsEnabled = false;
        bool mHalted = false;

        bool mBranchTaken = false;

	private:
		int HandleInstruction(OpCode instruction);

        void Load(OpCode op);

        template<typename Src, typename Dest>
        int LD(Src src, Dest dest) {}

#pragma region OpCodeFunctions
        /* ADC */
        void _OpcodeADC(Byte value);
        
        void OpcodeADC();
        void OpcodeADC(ByteReg reg);
        void OpcodeADC(Word addr);
        
        /* ADD */
        void _OpcodeADD(Byte reg, Byte value);
        
        void OpcodeADD_A();
        void OpcodeADD_A(ByteReg reg);
        void OpcodeADD_A(Word addr);
        
        void _OpcodeADD_HL(Word value);
        void OpcodeADD_HL(Register regPair);
        void OpcodeADD_HL(WordReg wordReg);
        
        void OpcodeADD_SP();
        
        void OpcodeADD_SIGNED();
        
        /* AND */
        void _OpcodeAND(Byte value);
        
        void OpcodeAND();
        void OpcodeAND(ByteReg reg);
        void OpcodeAND(Word addr);
        
        /* BIT */
        void _OpcodeBIT(Byte bit, Byte value);
        
        void OpcodeBIT(Byte bit, ByteReg reg);
        void OpcodeBIT(Byte bit, Word addr);
        
        /* CALL */
        void OpcodeCALL();
        void OpcodeCALL(Condition condition);
        
        /* CCF */
        void OpcodeCCF();
        
        /* CP */
        void _OpcodeCP(Byte value);
        
        void OpcodeCP();
        void OpcodeCP(ByteReg reg);
        void OpcodeCP(Word addr);
        
        /* CPL */
        void OpcodeCPL();
        
        /* DAA */
        void OpcodeDAA();
        
        /* DEC */
        void OpcodeDEC(ByteReg reg);
        void OpcodeDEC(Register reg);
        void OpcodeDEC(WordReg reg);
        void OpcodeDEC(Word addr);
        
        /* DI */
        void OpcodeDI();
        
        /* EI */
        void OpcodeEI();
        
        /* INC */
        void OpcodeINC(ByteReg reg);
        void OpcodeINC(Register reg);
        void OpcodeINC(WordReg reg);
        void OpcodeINC(Word addr);
        
        /* JP */
        void OpcodeJP();
        void OpcodeJP(Condition condition);
        void OpcodeJP(Word addr);
        
        /* JR */
        void OpcodeJR();
        void OpcodeJR(Condition condition);
        
        /* HALT */
        void OpcodeHALT();
        
        /* LD */
        void OpcodeLD(ByteReg reg);
        void OpcodeLD(ByteReg reg, ByteReg byteReg);
        void OpcodeLD(ByteReg reg, Word address);
        
        void OpcodeLD(Register reg);
        
        void OpcodeLD(WordReg reg);
        void OpcodeLD(WordReg reg, Register regPair);
        
        void OpcodeLD(Word address);
        void OpcodeLD(Word address, ByteReg byte_reg);
        void OpcodeLD(Word address, WordReg word_reg);
        
        // (nn), A
        void OpcodeLD_TO_ADDR(ByteReg reg);
        void OpcodeLD_FROM_ADDR(ByteReg reg);
        
        /* LDD */
        Byte _OpcodeLDD(Byte value);
        
        void OpcodeLDD(ByteReg reg, Word address);
        void OpcodeLDD(Word address, ByteReg reg);
        
        /* LDH */
        // A, (n)
        void OpcodeLDH_INTO_A();
        // (n), A
        void OpcodeLDH_INTO_DATA();
        // (reg), A
        void OpcodeLDH_INTO_C();
        // A, (reg)
        void OpcodeLDH_C_INTO_A();
        
        /* LDHL */
        void OpcodeLDHL();
        
        /* LDI */
        void OpcodeLDI(ByteReg reg, Word address);
        void OpcodeLDI(Word address, ByteReg reg);
        
        /* NOP */
        void OpcodeNOP();
        
        /* OR */
        void _OpcodeOR(Byte value);
        
        void OpcodeOR();
        void OpcodeOR(ByteReg reg);
        void OpcodeOR(Word addr);
        
        /* POP */
        void OpcodePOP(Register reg);
        
        /* PUSH */
        void OpcodePUSH(Register reg);
        
        /* RES */
        void OpcodeRES(Byte bit, ByteReg reg);
        void OpcodeRES(Byte bit, Word addr);
        
        /* RET */
        void OpcodeRET();
        void OpcodeRET(Condition condition);
        
        /* RETI */
        void OpcodeRETI();
        
        /* RL */
        Byte _OpcodeRL(Byte value);
        
        void OpcodeRLA();
        void OpcodeRL(ByteReg reg);
        void OpcodeRL(Word addr);
        
        /* RLC */
        Byte _OpcodeRLC(Byte value);
        
        void OpcodeRLCA();
        void OpcodeRLC(ByteReg reg);
        void OpcodeRLC(Word addr);
        
        /* RR */
        Byte _OpcodeRR(Byte value);
        
        void OpcodeRRA();
        void OpcodeRR(ByteReg reg);
        void OpcodeRR(Word addr);
        
        /* RRC */
        Byte _OpcodeRRC(Byte value);
        
        void OpcodeRRCA();
        void OpcodeRRC(ByteReg reg);
        void OpcodeRRC(Word addr);
        
        /* RST */
        void OpcodeRST(Byte offset);
        
        /* SBC */
        void _OpcodeSBC(Byte value);
        
        void OpcodeSBC();
        void OpcodeSBC(ByteReg reg);
        void OpcodeSBC(Word addr);
        
        /* SCF */
        void OpcodeSCF();
        
        /* SET */
        void OpcodeSET(Byte bit, ByteReg reg);
        void OpcodeSET(Byte bit, Word addr);
        
        /* SLA */
        Byte _OpcodeSLA(Byte value);
        
        void OpcodeSLA(ByteReg reg);
        void OpcodeSLA(Word addr);
        
        /* SRA */
        Byte _OpcodeSRA(Byte value);
        
        void OpcodeSRA(ByteReg reg);
        void OpcodeSRA(Word addr);
        
        /* SRL */
        Byte _OpcodeSRL(Byte value);
        
        void OpcodeSRL(ByteReg reg);
        void OpcodeSRL(Word addr);
        
        /* STOP */
        void OpcodeSTOP();
        
        /* SUB */
        void _OpcodeSUB(Byte value);
        
        void OpcodeSUB();
        void OpcodeSUB(ByteReg reg);
        void OpcodeSUB(Word addr);
        
        /* SWAP */
        Byte _OpcodeSWAP(Byte value);
        
        void OpcodeSWAP(ByteReg reg);
        void OpcodeSWAP(Word addr);
        
        /* XOR */
        void _OpcodeXOR(Byte value);
        
        void OpcodeXOR();
        void OpcodeXOR(ByteReg reg);
        void OpcodeXOR(Word addr);

		void Opcode00(); void Opcode01(); void Opcode02(); void Opcode03(); void Opcode04(); void Opcode05(); void Opcode06(); void Opcode07(); 
		void Opcode08(); void Opcode09(); void Opcode0A(); void Opcode0B(); void Opcode0C(); void Opcode0D(); void Opcode0E(); void Opcode0F();
		void Opcode10(); void Opcode11(); void Opcode12(); void Opcode13(); void Opcode14(); void Opcode15(); void Opcode16(); void Opcode17();
		void Opcode18(); void Opcode19(); void Opcode1A(); void Opcode1B(); void Opcode1C(); void Opcode1D(); void Opcode1E(); void Opcode1F();
		void Opcode20(); void Opcode21(); void Opcode22(); void Opcode23(); void Opcode24(); void Opcode25(); void Opcode26(); void Opcode27();
		void Opcode28(); void Opcode29(); void Opcode2A(); void Opcode2B(); void Opcode2C(); void Opcode2D(); void Opcode2E(); void Opcode2F();
		void Opcode30(); void Opcode31(); void Opcode32(); void Opcode33(); void Opcode34(); void Opcode35(); void Opcode36(); void Opcode37();
		void Opcode38(); void Opcode39(); void Opcode3A(); void Opcode3B(); void Opcode3C(); void Opcode3D(); void Opcode3E(); void Opcode3F();
		void Opcode40(); void Opcode41(); void Opcode42(); void Opcode43(); void Opcode44(); void Opcode45(); void Opcode46(); void Opcode47();
		void Opcode48(); void Opcode49(); void Opcode4A(); void Opcode4B(); void Opcode4C(); void Opcode4D(); void Opcode4E(); void Opcode4F();
		void Opcode50(); void Opcode51(); void Opcode52(); void Opcode53(); void Opcode54(); void Opcode55(); void Opcode56(); void Opcode57();
		void Opcode58(); void Opcode59(); void Opcode5A(); void Opcode5B(); void Opcode5C(); void Opcode5D(); void Opcode5E(); void Opcode5F();
		void Opcode60(); void Opcode61(); void Opcode62(); void Opcode63(); void Opcode64(); void Opcode65(); void Opcode66(); void Opcode67();
		void Opcode68(); void Opcode69(); void Opcode6A(); void Opcode6B(); void Opcode6C(); void Opcode6D(); void Opcode6E(); void Opcode6F();
		void Opcode70(); void Opcode71(); void Opcode72(); void Opcode73(); void Opcode74(); void Opcode75(); void Opcode76(); void Opcode77();
		void Opcode78(); void Opcode79(); void Opcode7A(); void Opcode7B(); void Opcode7C(); void Opcode7D(); void Opcode7E(); void Opcode7F();
		void Opcode80(); void Opcode81(); void Opcode82(); void Opcode83(); void Opcode84(); void Opcode85(); void Opcode86(); void Opcode87();
		void Opcode88(); void Opcode89(); void Opcode8A(); void Opcode8B(); void Opcode8C(); void Opcode8D(); void Opcode8E(); void Opcode8F();
		void Opcode90(); void Opcode91(); void Opcode92(); void Opcode93(); void Opcode94(); void Opcode95(); void Opcode96(); void Opcode97();
		void Opcode98(); void Opcode99(); void Opcode9A(); void Opcode9B(); void Opcode9C(); void Opcode9D(); void Opcode9E(); void Opcode9F();
		void OpcodeA0(); void OpcodeA1(); void OpcodeA2(); void OpcodeA3(); void OpcodeA4(); void OpcodeA5(); void OpcodeA6(); void OpcodeA7();
		void OpcodeA8(); void OpcodeA9(); void OpcodeAA(); void OpcodeAB(); void OpcodeAC(); void OpcodeAD(); void OpcodeAE(); void OpcodeAF();
		void OpcodeB0(); void OpcodeB1(); void OpcodeB2(); void OpcodeB3(); void OpcodeB4(); void OpcodeB5(); void OpcodeB6(); void OpcodeB7();
		void OpcodeB8(); void OpcodeB9(); void OpcodeBA(); void OpcodeBB(); void OpcodeBC(); void OpcodeBD(); void OpcodeBE(); void OpcodeBF();
		void OpcodeC0(); void OpcodeC1(); void OpcodeC2(); void OpcodeC3(); void OpcodeC4(); void OpcodeC5(); void OpcodeC6(); void OpcodeC7();
		void OpcodeC8(); void OpcodeC9(); void OpcodeCA(); void OpcodeCB(); void OpcodeCC(); void OpcodeCD(); void OpcodeCE(); void OpcodeCF();
		void OpcodeD0(); void OpcodeD1(); void OpcodeD2(); void OpcodeD3(); void OpcodeD4(); void OpcodeD5(); void OpcodeD6(); void OpcodeD7();
		void OpcodeD8(); void OpcodeD9(); void OpcodeDA(); void OpcodeDB(); void OpcodeDC(); void OpcodeDD(); void OpcodeDE(); void OpcodeDF();
		void OpcodeE0(); void OpcodeE1(); void OpcodeE2(); void OpcodeE3(); void OpcodeE4(); void OpcodeE5(); void OpcodeE6(); void OpcodeE7();
		void OpcodeE8(); void OpcodeE9(); void OpcodeEA(); void OpcodeEB(); void OpcodeEC(); void OpcodeED(); void OpcodeEE(); void OpcodeEF();
		void OpcodeF0(); void OpcodeF1(); void OpcodeF2(); void OpcodeF3(); void OpcodeF4(); void OpcodeF5(); void OpcodeF6(); void OpcodeF7();
		void OpcodeF8(); void OpcodeF9(); void OpcodeFA(); void OpcodeFB(); void OpcodeFC(); void OpcodeFD(); void OpcodeFE(); void OpcodeFF();
#pragma endregion
	
    };
}
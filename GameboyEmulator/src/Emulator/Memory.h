#pragma once

#include "Address.h"
#include "Register.h"
#include "RamBank.h"
#include "Cartridge.h"
#include "Interupts.h"

namespace GB {

    class Memory
    {
    private:
        GB_CONST USize _MemSize = 0x10000;

        GB_CONST Cartridge::BankMode MBC1 = Cartridge::MBC1;
        GB_CONST Cartridge::BankMode MBC2 = Cartridge::MBC2;

    public:
        Memory()
        {
            mMainMemory[0xFF05] = 0x00;    mMainMemory[0xFF06] = 0x00;    mMainMemory[0xFF07] = 0x00;    mMainMemory[0xFF10] = 0x80;
            mMainMemory[0xFF11] = 0xBF;    mMainMemory[0xFF12] = 0xF3;    mMainMemory[0xFF14] = 0xBF;    mMainMemory[0xFF16] = 0x3F;
            mMainMemory[0xFF17] = 0x00;    mMainMemory[0xFF19] = 0xBF;    mMainMemory[0xFF1A] = 0x7F;    mMainMemory[0xFF1B] = 0xFF;
            mMainMemory[0xFF1C] = 0x9F;    mMainMemory[0xFF1E] = 0xBF;    mMainMemory[0xFF20] = 0xFF;    mMainMemory[0xFF21] = 0x00;
            mMainMemory[0xFF22] = 0x00;    mMainMemory[0xFF23] = 0xBF;    mMainMemory[0xFF24] = 0x77;    mMainMemory[0xFF25] = 0xF3;
            mMainMemory[0xFF26] = 0xF1;    mMainMemory[0xFF40] = 0x91;    mMainMemory[0xFF42] = 0x00;    mMainMemory[0xFF43] = 0x00;
            mMainMemory[0xFF45] = 0x00;    mMainMemory[0xFF47] = 0xFC;    mMainMemory[0xFF48] = 0xFF;    mMainMemory[0xFF49] = 0xFF;
            mMainMemory[0xFF4A] = 0x00;    mMainMemory[0xFF4B] = 0x00;    mMainMemory[0xFFFF] = 0x00;
        }

        ~Memory()
        {
        }


    public:
        void updateTimers(USize cycles)
        {
            DividerRegister(cycles);

            // the clock must be enabled to update the clock
            if (IsClockEnabled())
            {
                mTimerCounter -= cycles;

                // enough cpu clock cycles have happened to update the timer
                if (mTimerCounter <= 0)
                {
                    // reset m_TimerTracer to the correct value
                    SetClockFreq();

                    // timer about to overflow
                    if (Read<Addr::TIMA>() == 255)
                    {
                        Write<Addr::TIMA>(Read<Addr::TMA>());
                        RequestInterupt<Interupts::V_BLNK>();
                    }
                    else
                        Write<Addr::TIMA>(Read<Addr::TIMA>() + 1);
                }
            }
        }

        void handleInterupts()
        {
            if (!mInteruptMaster)
                return;
            
            Byte req = Read<Addr::IF>();
            if (req == 0)
                return;

            Byte enabled = Read<Addr::IE>();
            for (Byte i = 0; i < 5; i++)
            {
                if (TestBit(req, i) && TestBit(enabled, i))
                    ServiceInterupt(i);
            }
        }

        // Main Access
    private:
        template<Word Address>
        Byte Read() const
        {
            if_c (Address >= 0x4000 && Address < 0x8000)
                return mROM.read<Address - 0x4000>();
            else if_c (Address >= 0xA000 && Address < 0xC000)
                return mRAMBanks.read<Address - 0xA000>();

            return mMainMemory[Address];
        }

        template<Word Address>
        void Write(Byte data)
        {
            GB_SASSERT(Address < 0xFEA0 || Address > 0xFF00, "Restricted memory!");

            if_c (Address < 0x8000)
                HandleBanking<Address>(data);
            else if_c (Address >= 0xA000 && Address < 0xC000)
                mRAMBanks.write<Address - 0xA000>(data);
            else if_c (Address >= 0xE000 && Address < 0xFE00)
                Write<Address - 0x2000>(data);
            else if_c (Address == Addr::TMC)
            {
                Byte currentFreq = GetClockFreq();
                mMainMemory[Addr::TMC] = data;
                Byte newFreq = GetClockFreq();
                if (currentFreq != newFreq)
                    SetClockFreq();
            }
            else if_c (Address == Addr::DIV)
                mMainMemory[Addr::DIV] = 0;
            else
                mMainMemory[Address] = data;
        }

        // Helpers
    private:
        Byte GetClockFreq() const { return Read<Addr::TMC>() & 0x3; }
        void SetClockFreq() 
        {
            Byte freq = GetClockFreq();
            switch (freq)
            {
            case 0: mTimerCounter = 1024; break; // freq 4096
            case 1: mTimerCounter = 16; break;   // freq 262144
            case 2: mTimerCounter = 64; break;   // freq 65536
            case 3: mTimerCounter = 256; break;  // freq 16382
            }
        }

        bool IsClockEnabled() const { return TestBit(Read<Addr::TMC>(), 2); }

        template<Byte _Bit>
        void RequestInterupt()
        {
            Byte req = Read<Addr::IF>();
            req = BitSet<_Bit>(req);
            Write<Addr:IF>(req);
        }
    
        // Internal
    private:
        template<Word Address>
        void HandleBanking(Byte data)
        {
            mBankMode = mROM.getBankMode();

            if_c(Address < 0x2000)
            {
                if (mBankMode)
                    RamBankEnable<Address>(data);
            }
            else if_c(Address >= 0x2000 && Address < 0x4000
            {
                if (mBankMode)
                    mROM.changeLoROMBank(data);
            }
            else if_c(Address >= 0x4000 && Address < 0x6000)
            {
                if (mBankMode & MBC1)
                {
                    if (mROMBanking)
                        mROM.changeHiROMBank(data);
                    else
                        mRAMBanks.bankChange(data);
                }
            }
            else if_c(Address >= 0x6000 && Address < 0x8000)
            {
                if (mBankMode & MBC1)
                    ChangeRomRamMode(data);
            }
        }

        template<Word Address>
        void RamBankEnable(Byte data)
        {
            if (mBankMode & MBC2 && Bit<Address, 4>::Eq(1))
                return;

            Byte testData = data & 0xF;
            if (testData == 0xA)
                mRAMBanks.enable();
            else
                mRAMBanks.disable();
        }

        void ChangeRomRamMode(Byte data)
        {
            Byte newData = data & 0x1;
            mROMBanking = (newData == 0);
            if (mROMBanking)
                mRAMBanks.resetBank();
        }

        void DividerRegister(USize cycles)
        {
            mDividerCounter += cycles;
            if (mDividerCounter >= 255)
            {
                mDividerCounter = 0;
                mMainMemory[Addr::DIV]++;
            }
        }

        void ServiceInterupt(Byte interupt)
        {
            mInteruptMaster = false;
            Byte req = Read<0xFF0F>();
            req = BitReset(req, interupt);
            Write<0xFF0F>(req);

            /// we must save the current execution address by pushing it onto the stack
            PushWordOntoStack(m_ProgramCounter);

            switch (interupt)
            {
            case 0: mProgramCounter = 0x40; break;
            case 1: mProgramCounter = 0x48; break;
            case 2: mProgramCounter = 0x50; break;
            case 4: mProgramCounter = 0x60; break;
            }
        }

    private:
        Byte mMainMemory[_MemSize];

        RamBank mRAMBanks;

        Cartridge mROM;
        Cartridge::BankMode mBankMode;

        bool mROMBanking = true;
        int mTimerCounter = 1024;
        int mDividerCounter = 0;

        bool mInteruptMaster = true;
    };

}
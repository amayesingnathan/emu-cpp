#pragma once

#include "Address.h"
#include "Cartridge/CartridgeManager.h"
#include "CPU/Register.h"

namespace GB {

    class CPU
    {
    private:
        GB_CONST USize _MemSize = 0x10000;

    public:
        CPU()
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

        ~CPU()
        {
        }


    public:
        void updateTimers(int cycles)
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
                    if (Read(Addr::TIMA) == 255)
                    {
                        Write(Addr::TIMA, Read(Addr::TMA));
                        RequestInterupt<Interupts::V_BLNK>();
                    }
                    else
                        Write(Addr::TIMA, Read(Addr::TIMA) + 1);
                }
            }
        }

        void handleInterupts()
        {
            if (!mInteruptMaster)
                return;
            
            Byte req = Read(Addr::IF);
            if (req == 0)
                return;

            Byte enabled = Read(Addr::IE);
            for (Byte i = 0; i < 5; i++)
            {
                if (TestBit(req, i) && TestBit(enabled, i))
                    ServiceInterupt(i);
            }
        }

        // Main Access
    private:
        Byte Read(Word address) const
        {
            if (address >= 0x4000 && address < 0x8000)
                return CartridgeManager::Read(address - 0x4000);
            else if (address >= 0xA000 && address < 0xC000)
                return mRAMBanks.read(address - 0xA000);

            return mMainMemory[address];
        }

        void Write(Word address, Byte data)
        {
            GB_ASSERT(address < 0xFEA0 || address > 0xFF00, "Restricted memory!");

            if (address < 0x8000)
                HandleBanking(address, data);
            else if (address >= 0xA000 && address < 0xC000)
                mRAMBanks.write(address - 0xA000, data);
            else if (address >= 0xE000 && address < 0xFE00)
                Write(address, data);
            else if (address == Addr::TMC)
            {
                Byte currentFreq = GetClockFreq();
                mMainMemory[Addr::TMC] = data;
                Byte newFreq = GetClockFreq();
                if (currentFreq != newFreq)
                    SetClockFreq();
            }
            else if (address == Addr::DIV)
                mMainMemory[Addr::DIV] = 0;
            else
                mMainMemory[address] = data;
        }

        // Helpers
    private:
        Byte GetClockFreq() const { return Read(Addr::TMC) & 0x3; }
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

        bool IsClockEnabled() const { return TestBit(Read(Addr::TMC), 2); }

        template<Byte _Bit>
        void RequestInterupt()
        {
            Byte req = Read(Addr::IF);
            req = BitSet<_Bit>(req);
            Write(Addr::IF);
        }

        // Stack Management
    private:
        void StackPush(Register data)
        {
            --mStackPointer;
            Write(mStackPointer.reg, data.hi);
            --mStackPointer;
            Write(mStackPointer.reg, data.lo);
        }
        Register StackPop()
        {
            Register data;

            data.lo = Read(mStackPointer.reg);
            ++mStackPointer;
            data.hi = Read(mStackPointer.reg);
            ++mStackPointer;

            return data;
        }
    
        // Internal
    private:
        void HandleBanking(Word address, Byte data)
        {
            mBankMode = CartridgeManager::BankMode();

            if (address < 0x2000)
            {
                if (mBankMode)
                    RamBankEnable(address, data);
            }
            else if (address >= 0x2000 && address < 0x4000)
            {
                if (mBankMode)
                    CartridgeManager::ChangeLoROMBank(data);
            }
            else if (address >= 0x4000 && address < 0x6000)
            {
                if (mBankMode & MBC1)
                {
                    if (mROMBanking)
                        CartridgeManager::ChangeHiROMBank(data);
                    else
                        mRAMBanks.bankChange(data);
                }
            }
            else if (address >= 0x6000 && address < 0x8000)
            {
                if (mBankMode & MBC1)
                    ChangeRomRamMode(data);
            }
        }

        void RamBankEnable(Word address, Byte data)
        {
            if (mBankMode & MBC2 && TestBit(address, 4))
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
            Byte req = Read(Addr::IF);
            req = BitReset(req, interupt);
            Write(0xFF0F, req);

            /// we must save the current execution address by pushing it onto the stack
            StackPush(mProgramCounter);

            switch (interupt)
            {
            case 0: mProgramCounter = 0x40; break;
            case 1: mProgramCounter = 0x48; break;
            case 2: mProgramCounter = 0x50; break;
            case 4: mProgramCounter = 0x60; break;
            }
        }

    private:
        Register mStackPointer = 0xFFFE;

        Byte mMainMemory[_MemSize];

        RamBank mRAMBanks;

        Cartridge::BankMode mBankMode;

        bool mROMBanking = true;

        int mTimerCounter = 1024;
        int mDividerCounter = 0;
    };

}
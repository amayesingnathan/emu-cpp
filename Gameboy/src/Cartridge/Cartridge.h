#pragma once

#include "Core/Types.h"

#include "MBC.h"

namespace GB {

    class Cartridge
    {
    public:
        GB_CONST USize _Size = 0x200000;

    public:
        Cartridge(std::string_view filename);
        ~Cartridge();

    public:
        Byte& read(Word address);
        void write(Word address, Byte data);

    //    void changeLoROMBank(Byte data)
    //    {
    //        if (mBankMode == MBC2)
    //        {
    //            mCurrentBank = data & 0xF;
    //            if (mCurrentBank == 0)
    //                mCurrentBank++;
    //            return;
    //        }

    //        Byte lower5 = data & 31;
    //        mCurrentBank &= 224;
    //        mCurrentBank |= lower5;
    //        if (mCurrentBank == 0)
    //            mCurrentBank++;
    //    }

    //    void changeHiROMBank(Byte data)
    //    {
    //        mCurrentBank &= 31;
    //        data &= 224;

    //        mCurrentBank |= data;
    //        if (mCurrentBank == 0)
    //            mCurrentBank++;
    //    }

    //    BankMode getBankMode() const { return mBankMode; }

    private:
        Byte* mROM = nullptr;
        MBC* mMBC = nullptr;
    };
}
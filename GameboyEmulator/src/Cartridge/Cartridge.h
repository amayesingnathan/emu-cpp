#pragma once

#include "Memory/Memory.h"

#include "MBC.h"

namespace GB {

    class Cartridge : public Memory
    {
    private:
        GB_CONST USize _ROMSize = 0x200000;

    public:
        Cartridge(std::string_view filename)
            : Memory(ROM), mCartridgeMemory((Byte*)::operator new(_ROMSize))
        {
            std::ifstream fileROM(filename.data(), std::ios::binary);
            fileROM.seekg(0, std::ios::end);

            USize cartridgeSize = fileROM.tellg();
            GB_ASSERT(cartridgeSize <= _ROMSize, "Gameboy cartridge is too large!");

            fileROM.read((char*)mCartridgeMemory, cartridgeSize);

            switch (mCartridgeMemory[0x147])
            {
            case 1:
            case 2:
            case 3:
                mMBC = new MBC1;
                break;

            case 5:
            case 6:
                mMBC = new MBC2;
                break;

            default: break;
            };
        }

        ~Cartridge()
        {
            delete mMBC;
            ::operator delete(mCartridgeMemory);
        }

    protected:
        Byte& GetMemBlock(Word address) override
        {
            if (!mMBC)
                return mCartridgeMemory[address];

            return mCartridgeMemory[mMBC->map(GetMode(), address)];
        }

        constexpr USize GetSize() const override { return _ROMSize; }

    //private:
    //    Byte read(Word address) const
    //    {
    //        return mCartridgeMemory[address + mCurrentBank * _ROMBankSize];
    //    }

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
        Byte* mCartridgeMemory = nullptr;
        MBC* mMBC = nullptr;

        friend class CartridgeManager;
    };
}
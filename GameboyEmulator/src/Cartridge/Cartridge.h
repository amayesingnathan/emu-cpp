#pragma once

#include "Core/Types.h"

namespace GB {

    class Cartridge
    {
    public:
        enum BankMode : Byte
        {
            NONE = 0,
            MBC1 = 1,
            MBC2 = 1 << 1
        };

    private:
        static constexpr USize _ROMSize = 0x200000;
        static constexpr USize _ROMBankSize = 0x4000;

    private:
        Cartridge() = default;
        Cartridge(std::string_view filename)
            : mCartridgeMemory((Byte*)::operator new(_ROMSize))
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
                mBankMode = MBC1;
                break;

            case 5:
            case 6:
                mBankMode = MBC2;
                break;

            default: break;
            };
        }

        ~Cartridge()
        {
            ::operator delete(mCartridgeMemory);
        }

        Cartridge(const Cartridge&) = delete;
        Cartridge(Cartridge&& other) noexcept
        {
            mCartridgeMemory = other.mCartridgeMemory;
            other.mCartridgeMemory = nullptr;
        }

    private:
        Byte read(Word address) const
        {
            return mCartridgeMemory[address + mCurrentBank * _ROMBankSize];
        }

        void changeLoROMBank(Byte data)
        {
            if (mBankMode == MBC2)
            {
                mCurrentBank = data & 0xF;
                if (mCurrentBank == 0)
                    mCurrentBank++;
                return;
            }

            Byte lower5 = data & 31;
            mCurrentBank &= 224;
            mCurrentBank |= lower5;
            if (mCurrentBank == 0)
                mCurrentBank++;
        }

        void changeHiROMBank(Byte data)
        {
            mCurrentBank &= 31;
            data &= 224;

            mCurrentBank |= data;
            if (mCurrentBank == 0)
                mCurrentBank++;
        }

        BankMode getBankMode() const { return mBankMode; }

    private:
        Byte* mCartridgeMemory = nullptr;
        BankMode mBankMode = NONE;
        Byte mCurrentBank = 1;

        friend class CartridgeManager;
    };
}
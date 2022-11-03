#pragma once

#include "Core/Types.h"

namespace GB {

    class RamBank
    {
    private:
        static constexpr USize _RamBankSize = 0x2000;

    public:
        RamBank() = default;

    public:
        void enable() { mEnabled = true; }
        void disable() { mEnabled = false; }

        operator bool() const { return mEnabled; }

        template<Word Address>
        Byte read() const
        {
            return mRamBanks[Address + mCurrentBank * _RamBankSize];
        }

        template<Word Address>
        void write(Byte data)
        {
            if (!mEnabled)
                return;

            mRamBanks[Address + mCurrentBank * _RamBankSize] = data;
        }

        void bankChange(Byte data)
        {
            mCurrentBank = data & 0x3;
        }

        void resetBank() { mCurrentBank = 0; }

    private:
        Byte mRamBanks[4 * _RamBankSize] = { 0 };
        Byte mCurrentBank = 0;

        bool mEnabled = false;
    };
}
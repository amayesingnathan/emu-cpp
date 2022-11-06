#pragma once

#include "Memory/Memory.h"

namespace GB {

    class MainMemory : public Memory
    {
    public:
        static constexpr USize _WRAMSize = 0x2000;

    public:
        MainMemory() : Memory(WRAM) {}

    protected:
        Byte& GetMemBlock(Address address) override { return mData[address]; }
        constexpr USize GetSize() const override { return _WRAMSize; }

    private:
        Byte mData[_WRAMSize];
    };
}
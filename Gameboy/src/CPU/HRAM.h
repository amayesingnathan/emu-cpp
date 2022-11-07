#pragma once

#include "Memory/Memory.h"

namespace GB {

    class StackMemory : public Memory
    {
    public:
        static constexpr USize _StackSize = 0x7E;

    public:
        StackMemory() : Memory(HRAM) {}

    protected:
        Byte& GetMemBlock(Address address) override { return mData[address]; }
        constexpr USize GetSize() const override { return _StackSize; }

    private:
        Byte mData[_StackSize];
    };
}
#pragma once

#include "Memory/Memory.h"

namespace GB {

    class WRAM : public Memory
    {
    public:
        static constexpr USize _WRAMSize = 0x2000;

    public:
        WRAM() : Memory(Type::WRAM) {}

    protected:
        Byte* GetMemBlock() override { return mData; }

    private:
        Byte mData[_WRAMSize];
    };
}
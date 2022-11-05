#pragma once

#include "Core/Types.h"

namespace GB {

    class WRAM
    {
    public:
        static constexpr USize _WRAMSize = 0x2000;


    private:
        Byte mData[_WRAMSize];

        friend class MemoryMap;
    };
}
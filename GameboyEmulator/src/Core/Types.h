#pragma once

#include "Base.h"

namespace GB {

    using Byte  = uint8_t;
    using UByte = int8_t;
    using Word  = uint16_t;
    using UWord = int16_t;

    using USize = size_t;

    template<Word _Value, Byte _Bit>
    struct Bit
    {
        GB_CONST bool Value = (_Value & (1 << _Bit)) >> _Bit;
    };

    inline static bool TestBit(Word value, Byte bit)
    {
        return (value & (1 << bit)) >> bit;
    }

    inline static Word BitSet(Word value, Byte bit)
    {
        return value | (1 << bit);
    }

    inline static Word BitReset(Word value, Byte bit)
    {
        return value & ~(1 << bit);
    }
}
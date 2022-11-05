#pragma once

#include "Base.h"

namespace GB {

    using Byte  = uint8_t;
    using SByte = int8_t;

    using Word  = uint16_t;

    using USize = size_t;

    using Flag = Byte;

    template<typename T>
    using Instance = std::unique_ptr<T>;

    template<typename T, typename... TArgs>
    Instance<T> MakeInstance(TArgs&&... args) { return make_unique<T>(std::forward<TArgs>(args)...); }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... TArgs>
    Ref<T> MakeRef(TArgs&&... args) { return make_shared<T>(std::forward<TArgs>(args)...); }

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
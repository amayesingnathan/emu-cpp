#pragma once

#include "Core/Types.h"

namespace GB {

    using Flag = Byte;
    static constexpr Flag _CarryFlag = 4;
    static constexpr Flag _HCarryFlag = 5;
    static constexpr Flag _SubtractFlag = 6;
    static constexpr Flag _ZeroFlag = 7;
}
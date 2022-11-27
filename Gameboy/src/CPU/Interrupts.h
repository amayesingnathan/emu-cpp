#pragma once

#include "Core/Types.h"

namespace GB {

    GB_CONST Byte _VBlankBit  = 0;
    GB_CONST Byte _LCDStatBit = 1;
    GB_CONST Byte _TimerBit   = 2;
    GB_CONST Byte _SerialBit  = 3;
    GB_CONST Byte _JoypadBit  = 4;

    enum class Interrupt : Byte
    {
        VBLANK = 0, LCD_STAT, TIMER, SERIAL, JOYPAD
    };
}
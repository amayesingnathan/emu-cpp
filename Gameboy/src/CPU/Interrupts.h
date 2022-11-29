#pragma once

#include "Core/Types.h"

namespace GB {

    GB_CONST Byte VBLANK_BIT     = 0;
    GB_CONST Byte LCD_STAT_BIT   = 1;
    GB_CONST Byte TIMER_BIT      = 2;
    GB_CONST Byte SERIAL_BIT     = 3;
    GB_CONST Byte JOYPAD_BIT     = 4;

    enum class Interrupt : Byte
    {
        VBLANK = 0, LCD_STAT, TIMER, SERIAL, JOYPAD
    };
}
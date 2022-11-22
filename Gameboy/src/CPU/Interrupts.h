#pragma once

#include "Core/Types.h"

namespace GB {

    enum class Interrupt : Byte
    {
        VBLANK = 0, LCD_STAT, TIMER, SERIAL, JOYPAD
    };
}
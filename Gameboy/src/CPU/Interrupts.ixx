export module Gameboy.CPU.Interrupts;

import Gameboy.Core.Types;

export namespace GB {

    constexpr Byte VBLANK_BIT = 0;
    constexpr Byte LCD_STAT_BIT = 1;
    constexpr Byte TIMER_BIT = 2;
    constexpr Byte SERIAL_BIT = 3;
    constexpr Byte JOYPAD_BIT = 4;

    enum class Interrupt : Byte
    {
        V_Blank = 0, LCD_Stat, Timer, Serial, Joypad
    };
}
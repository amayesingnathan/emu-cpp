export module Gameboy.Graphics.Flags;

import Gameboy.Core.Types;

export namespace GB {

    // OAM Flag Bits
    constexpr Byte PALETTE_NUM_BIT = 4;
    constexpr Byte X_FLIP_BIT = 5;
    constexpr Byte Y_FLIP_BIT = 6;
    constexpr Byte BACKGROUND_BIT = 7;


    // LCD Control (0xFF40) Bits
    constexpr Byte BG_ENABLE_BIT = 0;
    constexpr Byte OBJ_ENABLE_BIT = 1;
    constexpr Byte OBJ_SIZE_BIT = 2;
    constexpr Byte BG_TILEMAP_BIT = 3;
    constexpr Byte BG_TILEDATA_BIT = 4;
    constexpr Byte WINDOW_ENABLE_BIT = 5;
    constexpr Byte WINDOW_TILEMAP_BIT = 6;
    constexpr Byte LCD_ENABLE_BIT = 7;


    // LCD Status (0xFF41) Bits
    constexpr Byte LCD_MODE_BIT0 = 0;
    constexpr Byte LCD_MODE_BIT1 = 1;
    constexpr Byte LY_COMPARISON_BIT = 2;
    constexpr Byte LCD_INTERRUPT_BIT0 = 3;
    constexpr Byte LCD_INTERRUPT_BIT1 = 4;
    constexpr Byte LCD_INTERRUPT_BIT2 = 5;
    constexpr Byte LCD_INTERRUPT_BIT3 = 6;
}
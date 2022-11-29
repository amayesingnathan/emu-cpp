#pragma once

#include "Core/Types.h"

namespace GB {

    // OAM Flag Bits
    GB_CONST Byte PALETTE_NUM_BIT      = 4;
    GB_CONST Byte X_FLIP_BIT           = 5;
    GB_CONST Byte Y_FLIP_BIT           = 6;
    GB_CONST Byte BACKGROUND_BIT       = 7;
    

    // LCD Control (0xFF40) Bits
    GB_CONST Byte BG_ENABLE_BIT        = 0;
    GB_CONST Byte OBJ_ENABLE_BIT       = 1;
    GB_CONST Byte OBJ_SIZE_BIT         = 2;
    GB_CONST Byte BG_TILEMAP_BIT       = 3;
    GB_CONST Byte BG_TILEDATA_BIT      = 4;
    GB_CONST Byte WINDOW_ENABLE_BIT    = 5;
    GB_CONST Byte WINDOW_TILEMAP_BIT   = 6;
    GB_CONST Byte LCD_ENABLE_BIT       = 7;


    // LCD Status (0xFF41) Bits
    GB_CONST Byte LCD_MODE_BIT0        = 0;
    GB_CONST Byte LCD_MODE_BIT1        = 1;
    GB_CONST Byte LY_COMPARISON_BIT    = 2;
    GB_CONST Byte LCD_INTERRUPT_BIT0   = 3;
    GB_CONST Byte LCD_INTERRUPT_BIT1   = 4;
    GB_CONST Byte LCD_INTERRUPT_BIT2   = 5;
    GB_CONST Byte LCD_INTERRUPT_BIT3   = 6;
}
#pragma once

#include "Core/Flag.h"

namespace GB {

    // OAM Flag Bits
    GB_CONST Byte _PaletteNum    = 4;
    GB_CONST Byte _XFlipBit      = 5;
    GB_CONST Byte _YFlipBit      = 6;
    GB_CONST Byte _BackgroundBit = 7;
    

    // LCD Control (0xFF40) Bits
    GB_CONST Byte _BGEnableBit      = 0;
    GB_CONST Byte _OBJEnableBit     = 1;
    GB_CONST Byte _OBJSizeBit       = 2;
    GB_CONST Byte _BGTilemapBit     = 3;
    GB_CONST Byte _BGTileDataBit    = 4;
    GB_CONST Byte _WindowEnableBit  = 5;
    GB_CONST Byte _WindowTilemapBit = 6;
    GB_CONST Byte _LCDEnableBit     = 7;
}
#pragma once

#include "Core/Types.h"

namespace GB {

	GB_CONST Flag _Pallete1 = 0;
	GB_CONST Flag _Pallete2 = 1 << 0;
	GB_CONST Flag _Pallete3 = 1 << 1;
	GB_CONST Flag _VRAMBank = 1 << 2;
	GB_CONST Flag _PalleteN = 1 << 3;
	GB_CONST Flag _XFLip	= 1 << 4;
	GB_CONST Flag _YFlip	= 1 << 5;
	GB_CONST Flag _RenderPr = 1 << 6;

	struct Tile
	{
		Byte y, x;
		Byte tileID;
		Flag flags;
	};
}
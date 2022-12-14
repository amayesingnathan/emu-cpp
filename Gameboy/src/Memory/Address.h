#pragma once

#include "Core/Types.h"

namespace GB {

	namespace Addr {

		GB_CONST Word JOYP = 0xFF00;

		GB_CONST Word DIV  = 0xFF04;
		GB_CONST Word TIMA = 0xFF05;
		GB_CONST Word TMA  = 0xFF06;
		GB_CONST Word TMC  = 0xFF07;

		GB_CONST Word LCDC = 0xFF40;
		GB_CONST Word STAT = 0xFF41;
		GB_CONST Word SCY  = 0xFF42;
		GB_CONST Word SCX  = 0xFF43;
		GB_CONST Word LY   = 0xFF44;
		GB_CONST Word LYC  = 0xFF45;
		GB_CONST Word FMA  = 0xFF46;
		GB_CONST Word BGP  = 0xFF47;
		GB_CONST Word OBP0 = 0xFF48;
		GB_CONST Word OBP1 = 0xFF49;

		GB_CONST Word WY   = 0xFF4A;
		GB_CONST Word WX   = 0xFF4B;

		GB_CONST Word IF   = 0xFF0F;
		GB_CONST Word IE   = 0xFFFF;
	}
}
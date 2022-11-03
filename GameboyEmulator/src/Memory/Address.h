#pragma once

#include "Core/Types.h"

namespace GB {

	namespace Addr {

		GB_CONST Word DIV	= 0xFF04;
		GB_CONST Word TIMA	= 0xFF05;
		GB_CONST Word TMA	= 0xFF06;
		GB_CONST Word TMC	= 0xFF07;

		GB_CONST Word SCANL = 0xFF44;

		GB_CONST Word IF	= 0xFF0F;
		GB_CONST Word IE	= 0xFFFF;
	}
}
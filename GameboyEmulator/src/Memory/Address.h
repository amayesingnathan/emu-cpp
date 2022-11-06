#pragma once

#include "Core/Types.h"

namespace GB {

	using Address = Word;

	namespace Addr {

		GB_CONST Address DIV = 0xFF04;
		GB_CONST Address TIMA = 0xFF05;
		GB_CONST Address TMA = 0xFF06;
		GB_CONST Address TMC = 0xFF07;

		GB_CONST Address SCANL = 0xFF44;

		GB_CONST Address IF = 0xFF0F;
		GB_CONST Address IE	= 0xFFFF;
	}
}
#pragma once

#include "Core/Types.h"

namespace GB {

	struct OpCode
	{
		Byte data;

		OpCode(Byte _data) : data(_data) {}
		operator Byte() const { return data; }

		Byte x() const { return data >> 6; }
		Byte y() const { return (data >> 3) & 0x07; }
		Byte z() const { return data & 0x07; }

		Byte p() const { return (data >> 4) & 0x03; }
		Byte q() const { return (data >> 3) & 0x01; }

		Byte i() const { return (data >> 5) & 0x01; }
	};
}
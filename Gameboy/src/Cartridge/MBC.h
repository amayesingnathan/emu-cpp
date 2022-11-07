#pragma once

#include "Memory/Memory.h"

namespace GB {

	class MBC
	{
	public:
		virtual ~MBC() {}

		virtual Word map(Memory::Mode mode, Word address) = 0;
	};

	class MBC1 : public MBC
	{
		Word map(Memory::Mode mode, Word address) override { return 0; }
	};

	class MBC2 : public MBC
	{
		Word map(Memory::Mode mode, Word address) override { return 0; }
	};
}
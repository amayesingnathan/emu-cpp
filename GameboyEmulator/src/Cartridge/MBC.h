#pragma once

#include "Core/Types.h"

namespace GB {

	class MBC
	{
	public:
		virtual ~MBC() {}

		virtual Word map(Flag mode, Word address) = 0;
	};

	class MBC1 : public MBC
	{
		Word map(Flag mode, Word address) override {}
	};

	class MBC2 : public MBC
	{
		Word map(Flag mode, Word address) override {}
	};
}
#pragma once

#include "Core/Types.h"

namespace GB {

	class MBC
	{
	public:
		virtual ~MBC() {}

		virtual Word map(Word address) = 0;
	};

	class MBC1 : public MBC
	{

	};

	class MBC2 : public MBC
	{

	};
}
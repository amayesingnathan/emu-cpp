#pragma once

#include "Memory/Memory.h"

namespace GB {

	class OAM : public Memory
	{
	public:
		GB_CONST USize _OAMSize = 0xA0;

	protected:
		Byte& GetMemBlock(Word address) override { return mData[address]; }
		constexpr USize GetSize() const override { return _OAMSize; }

	private:
		Byte mData[_OAMSize];
	};
}
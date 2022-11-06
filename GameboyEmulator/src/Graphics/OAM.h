#pragma once

#include "Memory/Memory.h"

namespace GB {

	class SpriteMemory : public Memory
	{
	public:
		GB_CONST USize _OAMSize = 0xA0;

	public:
		SpriteMemory() : Memory(OAM) {}

	protected:
		Byte& GetMemBlock(Word address) override { return mData[address]; }
		constexpr USize GetSize() const override { return _OAMSize; }

	private:
		Byte mData[_OAMSize] = { 0 };
	};
}
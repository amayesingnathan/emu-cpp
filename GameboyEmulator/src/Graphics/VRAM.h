#pragma once

#include "Memory/Memory.h"

namespace GB {

	class VRAM : public Memory
	{
	public:
		static constexpr USize _VRAMSize = 0x2000;

	public:
		VRAM() : Memory(Memory::VRAM) {}

	protected:
		Byte& GetMemBlock(Word address) override { return mData[address]; }
		constexpr USize GetSize() const override { return _VRAMSize; }

	private:
		Byte mData[_VRAMSize];
	};
}
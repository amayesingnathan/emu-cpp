#pragma once

#include "Memory.h"

namespace GB {

	class AddressBus
	{
	public:
		static Byte Read(Word address);
		static void Write(Word address, Byte data);

	private:
		static void Register(Memory::Type type, Memory* memory) { sMemBlocks[type] = memory; }

	private:
		inline static std::array<Memory*, Memory::BLOCK_COUNT> sMemBlocks;

		friend Memory;
	};
}
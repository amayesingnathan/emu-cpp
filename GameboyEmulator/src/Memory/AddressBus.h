#pragma once

#include "Memory.h"
#include "Address.h"

namespace GB {

	struct BusState
	{
		bool bootstrap = true;
	};

	class AddressBus
	{
	public:
		static Byte Read(Address address);
		static void Write(Address address, Byte data);

	private:
		static void Register(Memory::Type type, Memory* memory) { sMemBlocks[type] = memory; }

	private:
		inline static std::array<Memory*, Memory::BLOCK_COUNT> sMemBlocks;
		inline static BusState sBusState;

		friend class Memory;
	};
}
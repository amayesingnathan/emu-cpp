#pragma once

#include "Memory.h"
#include "Address.h"

namespace GB {

	class Cartridge;

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
		static void Init(Cartridge* cartridge) { sCartridge = cartridge; }

	private:
		inline static Cartridge* sCartridge = nullptr;
		inline static BusState sBusState;

		friend class Gameboy;
	};
}
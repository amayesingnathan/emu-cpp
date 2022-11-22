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
		static Byte& Read(Address address);
		static void Write(Address address, Byte data);

		static void RequestInterupt(Byte bit);

	private:
		static void Init(Cartridge* cartridge) { sCartridge = cartridge; }

		static Byte& ReadIO(Address address);
		static void WriteIO(Address address, Byte data);

	private:
		inline static Cartridge* sCartridge = nullptr;
		inline static BusState sBusState;

		friend class Gameboy;
	};
}
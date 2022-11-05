#pragma once

#include "MemoryMap.h"

namespace GB {

	class AddressBus
	{
	public:
		static void Init(class Gameboy* context);

		static Byte Read(Word address);
		static void Write(Word address, Byte data);

	private:
		inline static MemoryMap sMemMap;

	};

	class BusConnection
	{
	public:

	private:
		
	};

	class System
	{

	};
}
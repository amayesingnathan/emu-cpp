#pragma once

#include "Core/Types.h"

namespace GB {

	class MemoryManager
	{
	public:
		enum Type
		{
			BOOTSTRAP, HRAM, WRAM, VRAM, OAM, ROM, BLOCK_COUNT
		};

	private:
		static void Init();
		static void Shutdown();

		friend class Gameboy;

	private:
		static Byte* Get(Type blockType);

		friend class Cartridge;

	private:
		static Byte& Get(Type blockType, USize address);

		friend class AddressBus;

	private:
		inline static Byte* sMemoryBlock = nullptr;

	};
}
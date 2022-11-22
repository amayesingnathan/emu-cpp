#pragma once

#include "Core/Types.h"

namespace GB {

	class MemoryManager
	{
	public:
		enum Type
		{
			BOOTSTRAP, HRAM, WRAM, VRAM, OAM, ROM, IO
		};

	private:
		static void Init();
		static void Shutdown();

		friend class Gameboy;

	private:
		static Byte* GetBlock(Type blockType);

		friend class Cartridge;

	private:
		static Byte& Get(Type blockType, USize address = 0);

		friend class AddressBus;

	private:
		inline static Byte* sMemoryBlock = nullptr;

	};
}
export module Gameboy.Memory.MemoryMapper;

export import <array>;
export import <ranges>;

import Gameboy.Core.Types;
import Gameboy.Cartridge;

namespace GB {

	// Memory Partition Sizes
	constexpr USize BOOT_ROM_SIZE	= 0x0100;
	constexpr USize STACK_SIZE		= 0x007E;
	constexpr USize IO_SIZE			= 0x0080 + 1; // Interrupt Enabled (0xFFFF) added here
	constexpr USize WRAM_SIZE		= 0x2000;
	constexpr USize VRAM_SIZE		= 0x2000;
	constexpr USize OAM_SIZE		= 0x00A0;
	constexpr USize MEMORY_SIZE		= BOOT_ROM_SIZE + STACK_SIZE + IO_SIZE + WRAM_SIZE + VRAM_SIZE + OAM_SIZE + ROM_MAX_SIZE + RAM_MAX_SIZE;

	// Memory Partition Offsets
	constexpr USize BOOT_OFFSET		= 0;
	constexpr USize HRAM_OFFSET		= BOOT_ROM_SIZE;
	constexpr USize IO_OFFSET		= HRAM_OFFSET + STACK_SIZE;
	constexpr USize WRAM_OFFSET		= IO_OFFSET + IO_SIZE;
	constexpr USize VRAM_OFFSET		= WRAM_OFFSET + WRAM_SIZE;
	constexpr USize OAM_OFFSET		= VRAM_OFFSET + VRAM_SIZE;
	constexpr USize ROM_OFFSET		= OAM_OFFSET + OAM_SIZE;
	constexpr USize CART_RAM_OFFSET	= ROM_OFFSET + ROM_MAX_SIZE;

	export class MemoryMapper
	{
	public:
		enum Type
		{
			BOOTSTRAP, HRAM, WRAM, VRAM, OAM, ROM, CART_RAM, IO
		};

		static void Init();
		static void Shutdown();

		static Byte& Read(Word address);
		static void Write(Word address, Byte val);

		static Byte* ReadROM();
		static void UpdateROMMap(Word startAddr);
		static void UpdateRAMMap(Word startAddr);

		static const auto& GetMapping() { return sMapping; }

	private:
		static Byte* GetBlock(Type blockType);
		static Byte* GetInternal(Type blockType, USize address = 0);

	private:
		inline static Emu::Impl<Byte> sMemoryBlock = nullptr;
		inline static std::vector<Byte*> sMapping = std::vector<Byte*>(0x10000);

		friend class Gameboy;
	};
}
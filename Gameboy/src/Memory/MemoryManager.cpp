#include "gbpch.h"
#include "MemoryManager.h"

#include "Cartridge/Cartridge.h"
#include "CPU/Boot.h"

namespace GB {

	// Memory Partition Sizes
	GB_CONST USize BOOT_ROM_SIZE   = 0x0100;
	GB_CONST USize STACK_SIZE	   = 0x007E;
	GB_CONST USize IO_SIZE		   = 0x0080 + 1; // Interrupt Enabled (0xFFFF) added here
	GB_CONST USize WRAM_SIZE	   = 0x2000;
	GB_CONST USize VRAM_SIZE	   = 0x2000;
	GB_CONST USize OAM_SIZE		   = 0x00A0;
	GB_CONST USize MEMORY_SIZE     = BOOT_ROM_SIZE + STACK_SIZE + IO_SIZE + WRAM_SIZE + VRAM_SIZE + OAM_SIZE + Cartridge::ROM_MAX_SIZE + Cartridge::RAM_MAX_SIZE;

	// Memory Partition Offsets
	GB_CONST USize BOOT_OFFSET		= 0;
	GB_CONST USize HRAM_OFFSET		= BOOT_ROM_SIZE;
	GB_CONST USize IO_OFFSET		= HRAM_OFFSET + STACK_SIZE;
	GB_CONST USize WRAM_OFFSET		= IO_OFFSET + IO_SIZE;
	GB_CONST USize VRAM_OFFSET		= WRAM_OFFSET + WRAM_SIZE;
	GB_CONST USize OAM_OFFSET		= VRAM_OFFSET + VRAM_SIZE;
	GB_CONST USize ROM_OFFSET		= OAM_OFFSET + OAM_SIZE;
	GB_CONST USize CART_RAM_OFFSET  = ROM_OFFSET + Cartridge::ROM_MAX_SIZE;

	void MemoryManager::Init()
	{
		GB_ASSERT(!sMemoryBlock, "Memory already initialised!");
		sMemoryBlock = (Byte*)::operator new(MEMORY_SIZE);
		memset(sMemoryBlock, 0xFF, MEMORY_SIZE);

		// Load Boot ROM into first 256 bytes
		memcpy(sMemoryBlock, Boot::ROM, BOOT_ROM_SIZE);
	}

	void MemoryManager::Shutdown()
	{
		GB_ASSERT(sMemoryBlock, "No memory to delete!");
		::operator delete(sMemoryBlock);
		sMemoryBlock = nullptr;
	}

	Byte* MemoryManager::GetBlock(Type blockType)
	{
		GB_ASSERT(sMemoryBlock, "Memory not initialised!");

		switch (blockType)
		{
		case BOOTSTRAP:
			return sMemoryBlock + BOOT_OFFSET;

		case HRAM:
			return sMemoryBlock + HRAM_OFFSET;

		case IO:
			return sMemoryBlock + IO_OFFSET;

		case WRAM:
			return sMemoryBlock + WRAM_OFFSET;

		case VRAM:
			return sMemoryBlock + VRAM_OFFSET;

		case OAM:
			return sMemoryBlock + OAM_OFFSET;

		case ROM:
			return sMemoryBlock + ROM_OFFSET;

		case CART_RAM:
			return sMemoryBlock + CART_RAM_OFFSET;

		default:
			GB_ASSERT(false, "Invalid mem block type!");
		}

		return nullptr;
	}

	Byte& MemoryManager::Get(Type blockType, USize address)
	{
		return *(GetBlock(blockType) + address);
	}
}
#include "gbpch.h"
#include "MemoryManager.h"

#include "Cartridge/Cartridge.h"
#include "CPU/Boot.h"

namespace GB {

	// Memory Partition Sizes
	GB_CONST USize _BootROMSize = 0x0100;
	GB_CONST USize _StackSize	= 0x007E;
	GB_CONST USize _WRAMSize	= 0x2000;
	GB_CONST USize _VRAMSize	= 0x2000;
	GB_CONST USize _OAMSize		= 0x00A0;
	GB_CONST USize _IOSize		= 0x0080 + 1; // Interrupt Enabled (0xFFFF) added here
	GB_CONST USize _MemorySize = _BootROMSize + _StackSize + _IOSize + _WRAMSize + _VRAMSize + _OAMSize + Cartridge::_Size;

	// Memory Partition Offsets
	GB_CONST USize _BootOffset = 0;
	GB_CONST USize _HRAMOffset = _BootROMSize;
	GB_CONST USize _IOOffset   = _BootROMSize + _IOSize;
	GB_CONST USize _WRAMOffset = _BootROMSize + _IOSize + _StackSize;
	GB_CONST USize _VRAMOffset = _BootROMSize + _IOSize + _StackSize + _WRAMSize;
	GB_CONST USize _OAMOffset  = _BootROMSize + _IOSize + _StackSize + _WRAMSize + _VRAMSize;
	GB_CONST USize _ROMOffset  = _BootROMSize + _IOSize + _StackSize + _WRAMSize + _VRAMSize + _OAMSize;

	void MemoryManager::Init()
	{
		GB_ASSERT(!sMemoryBlock, "Memory already initialised!");
		sMemoryBlock = (Byte*)::operator new(_MemorySize);
		memset(sMemoryBlock, 0, _MemorySize);

		// Load Boot ROM into first 256 bytes
		memcpy(sMemoryBlock, Boot::_BootROM, _BootROMSize);
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
			return sMemoryBlock + _BootOffset;

		case HRAM:
			return sMemoryBlock + _HRAMOffset;

		case IO:
			return sMemoryBlock + _IOOffset;

		case WRAM:
			return sMemoryBlock + _WRAMOffset;

		case VRAM:
			return sMemoryBlock + _VRAMOffset;

		case OAM:
			return sMemoryBlock + _OAMOffset;

		case ROM:
			return sMemoryBlock + _ROMOffset;

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
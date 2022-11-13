#include "gbpch.h"
#include "MemoryManager.h"

#include "Cartridge/Cartridge.h"

namespace GB {

	// Memory Partition Sizes
	GB_CONST USize _BootROMSize = 0x0100;
	GB_CONST USize _StackSize	= 0x007E;
	GB_CONST USize _WRAMSize	= 0x2000;
	GB_CONST USize _VRAMSize	= 0x2000;
	GB_CONST USize _OAMSize		= 0x00A0;
	GB_CONST USize _MemorySize = _BootROMSize + _StackSize + _WRAMSize + _VRAMSize + _OAMSize + Cartridge::_Size;

	// Memory Partition Offsets
	GB_CONST USize _BootOffset = 0;
	GB_CONST USize _HRAMOffset = _BootROMSize;
	GB_CONST USize _WRAMOffset = _BootROMSize + _StackSize;
	GB_CONST USize _VRAMOffset = _BootROMSize + _StackSize + _WRAMSize;
	GB_CONST USize _OAMOffset  = _BootROMSize + _StackSize + _WRAMSize + _VRAMSize;
	GB_CONST USize _ROMOffset  = _BootROMSize + _StackSize + _WRAMSize + _VRAMSize + _OAMSize;

	void MemoryManager::Init()
	{
		GB_ASSERT(!sMemoryBlock, "Memory already initialised!");
		sMemoryBlock = (Byte*)::operator new(_MemorySize);

		// Load Boot ROM into first 256 bytes
		std::ifstream bootROM("BootROM.gb", std::ios::binary);
		GB_ASSERT(bootROM.is_open(), "Could not locate boot ROM!");

		bootROM.seekg(0, std::ios::end);
		USize cartridgeSize = bootROM.tellg();
		GB_ASSERT(cartridgeSize == _BootROMSize, "Error! Boot ROM is not 256B");

		bootROM.seekg(0);
		bootROM.read((char*)sMemoryBlock, _BootROMSize);
	}

	void MemoryManager::Shutdown()
	{
		GB_ASSERT(sMemoryBlock, "No memory to delete!");
		::operator delete(sMemoryBlock);
		sMemoryBlock = nullptr;
	}

	Byte* MemoryManager::Get(Type blockType)
	{
		GB_ASSERT(sMemoryBlock, "Memory not initialised!");

		switch (blockType)
		{
		case BOOTSTRAP:
			return sMemoryBlock + _BootOffset;

		case HRAM:
			return sMemoryBlock + _HRAMOffset;

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
		return *(Get(blockType) + address);
	}
}
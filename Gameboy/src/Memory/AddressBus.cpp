#include "gbpch.h"
#include "AddressBus.h"

#include "Address.h"
#include "MemoryManager.h"

#include "Cartridge/Cartridge.h"

namespace GB {

	Byte AddressBus::Read(Address address)
	{
		GB_ASSERT(address < 0xFEA0 || address > 0xFF00, "Restricted memory!");

		if (sBusState.bootstrap && address < 0x0100)
			return MemoryManager::Get(MemoryManager::BOOTSTRAP, address);
		else if (address < 0x8000)						// ROM
			return sCartridge->read(address);
		else if (address >= 0x8000 && address < 0xA000) // VRAM
			return MemoryManager::Get(MemoryManager::VRAM, address - 0x8000);
		else if (address >= 0xA000 && address < 0xC000) // Switchable RAM banks in cartridge
			return sCartridge->read(address);
		else if (address >= 0xC000 && address < 0xE000) // WRAM
			return MemoryManager::Get(MemoryManager::WRAM, address - 0xC000);
		else if (address >= 0xE000 && address < 0xFE00) // Echo of 0xC000 - 0xDFFF
			return MemoryManager::Get(MemoryManager::WRAM, address - 0xE000);
		else if (address >= 0xFE00 && address < 0xFEA0) // Sprite Attribute Table (OAM)
			return MemoryManager::Get(MemoryManager::OAM, address - 0xFE00);
		else if (address >= 0xFF80 && address < 0xFFFF) // High RAM (Stack)
			return MemoryManager::Get(MemoryManager::HRAM, address - 0xFF80);
		else if (address == Addr::TMC)
			GB_ASSERT(false, "Not yet mapped!");
		else if (address == Addr::DIV)
			GB_ASSERT(false, "Not yet mapped!");
		else
			GB_ASSERT(false, "Not yet mapped!");

		return 0;
	}

	void AddressBus::Write(Address address, Byte data)
	{
        GB_ASSERT(address < 0xFEA0 || address > 0xFF00, "Restricted memory!");
        GB_ASSERT(!sBusState.bootstrap || address >= 0x0100, "Cannot write to boot ROM except to disable it!");

		if (sBusState.bootstrap && address == 0xFF50)
			sBusState.bootstrap = false;
		else if (address < 0x8000)						// ROM
			sCartridge->write(address, data);
		else if (address >= 0x8000 && address < 0xA000) // VRAM
			MemoryManager::Get(MemoryManager::VRAM, address - 0x8000) = data;
		else if (address >= 0xA000 && address < 0xC000) // Switchable RAM banks in cartridge
			sCartridge->write(address, data);
		else if (address >= 0xC000 && address < 0xE000) // WRAM
			MemoryManager::Get(MemoryManager::WRAM, address - 0xC000) = data;
		else if (address >= 0xE000 && address < 0xFE00) // Echo of 0xC000 - 0xDFFF
			MemoryManager::Get(MemoryManager::WRAM, address - 0xE000) = data;
		else if (address >= 0xFE00 && address < 0xFEA0) // Sprite Attribute Table (OAM)
			MemoryManager::Get(MemoryManager::OAM, address - 0xFE00) = data;
		else if (address >= 0xFF00 && address < 0xFF80) // IO Registers
			; // IO Register still to be mapped!
		else if (address >= 0xFF80 && address < 0xFFFF) // High RAM (Stack)
			MemoryManager::Get(MemoryManager::HRAM, address - 0xFF80) = data;
		else if (address == Addr::TMC)
			GB_ASSERT(false, "Not yet mapped!");
		else if (address == Addr::DIV)
			GB_ASSERT(false, "Not yet mapped!");
		else
			GB_ASSERT(false, "Not yet mapped!");
	}
}
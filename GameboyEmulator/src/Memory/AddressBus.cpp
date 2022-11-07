#include "gbpch.h"
#include "AddressBus.h"

#include "Address.h"

namespace GB {

	Byte AddressBus::Read(Address address)
	{
		GB_ASSERT(address < 0xFEA0 || address > 0xFF00, "Restricted memory!");

		if (sBusState.bootstrap && address < 0x0100)
			return sMemBlocks[Memory::BOOTSTRAP]->read(address);
		else if (address < 0x8000)							// ROM
			return sMemBlocks[Memory::ROM]->read(address);
		else if (address >= 0x8000 && address < 0xA000) // VRAM
			return sMemBlocks[Memory::VRAM]->read(address - 0x8000);
		else if (address >= 0xA000 && address < 0xC000) // Switchable RAM banks in cartridge
			return sMemBlocks[Memory::ROM]->read(address);
		else if (address >= 0xC000 && address < 0xE000) // WRAM
			return sMemBlocks[Memory::WRAM]->read(address - 0xC000);
		else if (address >= 0xE000 && address < 0xFE00) // Echo of 0xC000 - 0xDFFF
			return sMemBlocks[Memory::WRAM]->read(address - 0xC000);
		else if (address >= 0xFE00 && address < 0xFEA0) // Sprite Attribute Table (OAM)
			return sMemBlocks[Memory::OAM]->read(address - 0xFE00);
		else if (address >= 0xFF80 && address < 0xFFFF) // High RAM (Stack)
			return sMemBlocks[Memory::HRAM]->read(address - 0xFF80);
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
		else if (sBusState.bootstrap && address < 0x0100)
			sMemBlocks[Memory::BOOTSTRAP]->write(address, data);
		else if (address < 0x8000)							// ROM
			sMemBlocks[Memory::ROM]->write(address, data);
		else if (address >= 0x8000 && address < 0xA000) // VRAM
			sMemBlocks[Memory::VRAM]->write(address - 0x8000, data);
		else if (address >= 0xA000 && address < 0xC000) // Switchable RAM banks in cartridge
			sMemBlocks[Memory::ROM]->write(address, data);
		else if (address >= 0xC000 && address < 0xE000) // WRAM
			sMemBlocks[Memory::WRAM]->write(address - 0xC000, data);
		else if (address >= 0xE000 && address < 0xFE00) // Echo of 0xC000 - 0xDFFF
			sMemBlocks[Memory::WRAM]->write(address - 0xC000, data);
		else if (address >= 0xFE00 && address < 0xFEA0) // Sprite Attribute Table (OAM)
			sMemBlocks[Memory::OAM]->write(address - 0xFE00, data);
		else if (address >= 0xFF80 && address < 0xFFFF) // High RAM (Stack)
			sMemBlocks[Memory::HRAM]->write(address - 0xFF80, data);
		else if (address == Addr::TMC)
			GB_ASSERT(false, "Not yet mapped!");
		else if (address == Addr::DIV)
			GB_ASSERT(false, "Not yet mapped!");
		else
			GB_ASSERT(false, "Not yet mapped!");
	}
}
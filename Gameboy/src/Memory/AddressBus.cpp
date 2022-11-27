#include "gbpch.h"
#include "AddressBus.h"

#include "Address.h"
#include "MemoryManager.h"

#include "Cartridge/Cartridge.h"
#include "Graphics/Screen.h"

namespace GB {

	Byte& AddressBus::Read(Word address)
	{
		if (address >= 0xFEA0 && address < 0xFF00)
			return sErrorByte; // Warn: restricted memory!

		if (sBusState.bootstrap && address < 0x0100)
			return MemoryManager::Get(MemoryManager::BOOTSTRAP, address);
		else if (address < 0x8000)						// ROM - may have memory bank controller to handle reading
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
		else if (address >= 0xFF00 && address < 0xFF80) // IO Registers
			return MemoryManager::Get(MemoryManager::IO, address - 0xFF00);
		else if (address >= 0xFF80 && address < 0xFFFF) // High RAM (Stack)
			return MemoryManager::Get(MemoryManager::HRAM, address - 0xFF80);
		else if (address == 0xFFFF)						// Interrupt Enable Register
			return MemoryManager::Get(MemoryManager::IO, 0x80);

		GB_ASSERT(false, "Cannot map to this memory!");
		return sErrorByte;
	}

	void AddressBus::Write(Word address, Byte data)
	{
		if (address >= 0xFEA0 && address < 0xFF00)
			return; // Warn: restricted memory!

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
			WriteIO(address, data); 
		else if (address >= 0xFF80 && address < 0xFFFF) // High RAM (Stack)
			MemoryManager::Get(MemoryManager::HRAM, address - 0xFF80) = data;
		else if (address == 0xFFFF)						// Interrupt Enable Register
			MemoryManager::Get(MemoryManager::IO, 0x80) = data;
		else
			GB_ASSERT(false, "Cannot map to this memory!");
	}

	const Sprite& AddressBus::ReadSprite(Word address)
	{
		GB_ASSERT(address >= 0xFE00 || address < 0xFEA0, "Not sprite memory!");

		return *(Sprite*)&MemoryManager::Get(MemoryManager::OAM, address - 0xFE00);
	}

	Byte& AddressBus::ReadIO(Word address)
	{
		switch (address)
		{
		// Undocumented IO registers
		case 0xFF08: case 0xFF09: case 0xFF27: case 0xFF28: case 0xFF29:
		case 0xFF4C: case 0xFF4D: case 0xFF4E: case 0xFF56: case 0xFF57:
		case 0xFF58: case 0xFF59: case 0xFF5A: case 0xFF5B: case 0xFF5C:
		case 0xFF5D: case 0xFF5E: case 0xFF5F: case 0xFF60: case 0xFF61:
		case 0xFF62: case 0xFF63: case 0xFF64: case 0xFF65: case 0xFF66:
		case 0xFF67: case 0xFF71: case 0xFF72: case 0xFF73: case 0xFF74:
		case 0xFF75: case 0xFF78: case 0xFF79:
			return sErrorByte;

		default:
			return MemoryManager::Get(MemoryManager::IO, address - 0xFF00);
		}
	}

	void AddressBus::WriteIO(Word address, Byte data)
	{
		switch (address)
		{
		case Addr::DIV:
			MemoryManager::Get(MemoryManager::IO, Addr::DIV - 0xFF00) = 0;
			break;

		case Addr::TMC:
		{
			Byte& tmc = MemoryManager::Get(MemoryManager::IO, Addr::TMC - 0xFF00);
			Byte currentFreq = tmc & 0x3;
			tmc = data;
			Byte newFreq = tmc & 0x3;

			if (currentFreq != newFreq)
				SetClockFreq(newFreq);
			break;
		}

		case Addr::LYC:
			MemoryManager::Get(MemoryManager::IO, Addr::LYC - 0xFF00) = 0;
			break;

		// Undocumented IO registers
		case 0xFF08: case 0xFF09: case 0xFF27: case 0xFF28: case 0xFF29:
		case 0xFF4C: case 0xFF4D: case 0xFF4E: case 0xFF56: case 0xFF57:
		case 0xFF58: case 0xFF59: case 0xFF5A: case 0xFF5B: case 0xFF5C:
		case 0xFF5D: case 0xFF5E: case 0xFF5F: case 0xFF60: case 0xFF61:
		case 0xFF62: case 0xFF63: case 0xFF64: case 0xFF65: case 0xFF66:
		case 0xFF67: case 0xFF71: case 0xFF72: case 0xFF73: case 0xFF74:
		case 0xFF75: case 0xFF78: case 0xFF79:
			break;

		default:
			MemoryManager::Get(MemoryManager::IO, address - 0xFF00) = data;
			break;
		}
	}

	void AddressBus::SetClockFreq(Byte freq)
	{
		switch (freq)
		{
		case 0: *sClockSpeed = 1024; break; // freq 4096
		case 1: *sClockSpeed = 16; break;// freq 262144
		case 2: *sClockSpeed = 64; break;// freq 65536
		case 3: *sClockSpeed = 256; break;// freq 16382
		}
	}

	void AddressBus::RequestInterrupt(Interrupt interrupt)
	{
		BitField req = Read(Addr::IF);
		req.set((Byte)interrupt);
		Write(Addr::IF, req);
	}
}
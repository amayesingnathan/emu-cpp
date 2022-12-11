#include "gbpch.h"
#include "AddressBus.h"

#include "Address.h"
#include "MemoryManager.h"

#include "Cartridge/Cartridge.h"
#include "CPU/CPU.h"
#include "Graphics/PPU.h"

namespace GB {

	void AddressBus::Init(CPU* cpu, PPU* ppu, Cartridge* cartridge)
	{
		GB_ASSERT(!sInitialised, "Address Bus already intialised!");
		Get().init(cpu, ppu, cartridge);
		sInitialised = true;
	}

	void AddressBus::Shutdown()
	{
		GB_ASSERT(sInitialised, "Address Bus not intialised!");
		Get().shutdown();
		sInitialised = false;
	}

	Byte& AddressBus::Read(Word address)
	{
		GB_ASSERT(sInitialised, "Address Bus not intialised!");
		AddressBus& bus = Get();

		if (address >= 0xFEA0 && address < 0xFF00)
		{
			EMU_WARN("Reading from restricted memory!");
			return bus.mErrorByte;
		}

		if (bus.mBusState.bootstrap && address < 0x0100)	// Boot ROM
			return MemoryManager::Get(MemoryManager::BOOTSTRAP, address);
		else if (address < 0x8000)						// ROM - may have memory bank controller to handle reading
			return bus.mCartridge->read(address);
		else if (address >= 0x8000 && address < 0xA000) // VRAM
			return MemoryManager::Get(MemoryManager::VRAM, address - 0x8000);
		else if (address >= 0xA000 && address < 0xC000) // Switchable RAM banks in cartridge
			return bus.mCartridge->read(address);
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
		return bus.mErrorByte;
	}

	void AddressBus::Write(Word address, Byte data)
	{
		GB_ASSERT(sInitialised, "Address Bus not intialised!");
		AddressBus& bus = Get();

		if (address >= 0xFEA0 && address < 0xFF00)
		{
			EMU_ERROR("Writing to restricted memory!");
			return;
		}

        GB_ASSERT(!bus.mBusState.bootstrap || address >= 0x0100, "Cannot write to boot ROM except to disable it!");

		if (bus.mBusState.bootstrap && address == 0xFF50)
			bus.mBusState.bootstrap = false;
		else if (address < 0x8000)						// ROM
			bus.mCartridge->write(address, data);
		else if (address >= 0x8000 && address < 0xA000) // VRAM
			MemoryManager::Get(MemoryManager::VRAM, address - 0x8000) = data;
		else if (address >= 0xA000 && address < 0xC000) // Switchable RAM banks in cartridge
			bus.mCartridge->write(address, data);
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
		AddressBus& bus = Get();

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
			EMU_WARN("Reading from undocumented IO Register: {:X}", address);
			return bus.mErrorByte;

		default:
			return MemoryManager::Get(MemoryManager::IO, address - 0xFF00);
		}
	}

	void AddressBus::WriteIO(Word address, Byte data)
	{
		AddressBus& bus = Get();

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

		case Addr::LCDC:
		{
			Byte& lcdc = MemoryManager::Get(MemoryManager::IO, Addr::LCDC - 0xFF00);

			BitField lcdControl = lcdc;
			bool lcdPrevEnabled = lcdControl.bit(LCD_ENABLE_BIT);

			lcdc = data;
			bool lcdEnabled = BitField(lcdc).bit(LCD_ENABLE_BIT);

			if (!lcdPrevEnabled || lcdEnabled)
				break;

			bus.mPPU->DisableLCD();
			bus.mPPU->mDisplayPB->set({ 0, 0, 0, 255 });
			break;
		}

		case Addr::LCDS:
		{
			Byte& lcdStatus = MemoryManager::Get(MemoryManager::IO, Addr::LCDS - 0xFF00);
			lcdStatus = (data & 0xF8) | (lcdStatus & 0x07);
			break;
		}

		case Addr::LY:
			MemoryManager::Get(MemoryManager::IO, Addr::LY - 0xFF00) = 0;
			break;

		case Addr::FMA:
			DoDMATransfer(data);
			break;

		// Undocumented IO registers
		case 0xFF08: case 0xFF09: case 0xFF27: case 0xFF28: case 0xFF29:
		case 0xFF4C: case 0xFF4D: case 0xFF4E: case 0xFF56: case 0xFF57:
		case 0xFF58: case 0xFF59: case 0xFF5A: case 0xFF5B: case 0xFF5C:
		case 0xFF5D: case 0xFF5E: case 0xFF5F: case 0xFF60: case 0xFF61:
		case 0xFF62: case 0xFF63: case 0xFF64: case 0xFF65: case 0xFF66:
		case 0xFF67: case 0xFF71: case 0xFF72: case 0xFF73: case 0xFF74:
		case 0xFF75: case 0xFF78: case 0xFF79:
			EMU_ERROR("Writing to undocumented IO Register: {:X}", address);
			break;

		default:
			MemoryManager::Get(MemoryManager::IO, address - 0xFF00) = data;
			break;
		}
	}

	void AddressBus::SetClockFreq(Byte freq)
	{
		GB_ASSERT(sInitialised, "Address Bus not intialised!");
		AddressBus& bus = Get();

		switch (freq)
		{
		case 0: bus.mCPU->mCurrentClockSpeed = CPU::TMC0; break;
		case 1: bus.mCPU->mCurrentClockSpeed = CPU::TMC1; break;
		case 2: bus.mCPU->mCurrentClockSpeed = CPU::TMC2; break;
		case 3: bus.mCPU->mCurrentClockSpeed = CPU::TMC3; break;
		}
	}

	void AddressBus::DoDMATransfer(Byte data)
	{
		Word address = data << 8;
		Byte* src = &Read(address);
		Byte* dest = MemoryManager::GetBlock(MemoryManager::OAM);
		memcpy(dest, src, 0xA0);
	}

	void AddressBus::RequestInterrupt(Interrupt interrupt)
	{
		BitField req = Read(Addr::IF);
		req.set((Byte)interrupt);
		Write(Addr::IF, req);
	}
}
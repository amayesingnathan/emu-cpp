module Gameboy.Memory.MemoryMapper;

import Gameboy.CPU.Boot;

namespace GB {

	void MemoryMapper::Init()
	{
		Emu::Assert(!sMemoryBlock, "Memory already initialised!");
		sMemoryBlock = Emu::Impl<Byte>((Byte*)::operator new(MEMORY_SIZE));
		memset(sMemoryBlock.get(), 0xFF, MEMORY_SIZE);

		// Load Boot ROM into first 256 bytes
		memcpy(sMemoryBlock.get(), Boot::ROM, BOOT_ROM_SIZE);

		for (USize i = 0; i < 0x10000; i++)
		{
			if (i < 0x0100)	// Boot ROM
				sMapping[i] = GetInternal(BOOTSTRAP, i);
			else if (i < 0x8000)						// ROM - may have memory bank controller to handle reading
				sMapping[i] = GetInternal(ROM, i);
			else if (i >= 0x8000 && i < 0xA000)			// VRAM
				sMapping[i] = GetInternal(VRAM, i - 0x8000);
			else if (i >= 0xA000 && i < 0xC000)			// Switchable RAM banks in cartridge
				sMapping[i] = GetInternal(CART_RAM, i - 0xA000);
			else if (i >= 0xC000 && i < 0xE000)			// WRAM
				sMapping[i] = GetInternal(WRAM, i - 0xC000);
			else if (i >= 0xE000 && i < 0xFE00)			// Echo of 0xC000 - 0xDFFF
				sMapping[i] = GetInternal(WRAM, i - 0xE000);
			else if (i >= 0xFE00 && i < 0xFEA0)			// Sprite Attribute Table (OAM)
				sMapping[i] = GetInternal(OAM, i - 0xFE00);
			else if (i >= 0xFF00 && i < 0xFF80)			// IO Registers
				sMapping[i] = GetInternal(IO, i - 0xFF00);
			else if (i >= 0xFF80 && i < 0xFFFF)			// High RAM (Stack)
				sMapping[i] = GetInternal(HRAM, i - 0xFF80);
			else if (i == 0xFFFF)						// Interrupt Enable Register
				sMapping[i] = GetInternal(IO, 0x80);
		}
	}

	void MemoryMapper::Shutdown()
	{
		sMemoryBlock.reset();
		memset(sMapping, 0, 0x10000);
	}

	Byte& MemoryMapper::Read(Word address)
	{
		return *sMapping[address];
	}

	void MemoryMapper::Write(Word address, Byte val)
	{
		*sMapping[address] = val;
	}

	Byte* MemoryMapper::ReadROM()
	{
		return GetBlock(ROM);
	}

	void MemoryMapper::UpdateROMMap(Word startAddr)
	{
		Byte* mappedROM = GetInternal(ROM, startAddr);

		for (USize i = 0x4000; i < 0x8000; i++)
			sMapping[i] = mappedROM++;
	}

	void MemoryMapper::UpdateRAMMap(Word startAddr)
	{
		Byte* mappedROM = GetInternal(CART_RAM, startAddr);

		for (USize i = 0; i < 0x2000; i++)
			sMapping[0xA000 + i] = &mappedROM[i];
	}

	Byte* MemoryMapper::GetBlock(Type blockType)
	{
		Emu::Assert(sMemoryBlock.get(), "Memory not initialised!");

		switch (blockType)
		{
		case BOOTSTRAP:
			return sMemoryBlock.get() + BOOT_OFFSET;

		case HRAM:
			return sMemoryBlock.get() + HRAM_OFFSET;

		case IO:
			return sMemoryBlock.get() + IO_OFFSET;

		case WRAM:
			return sMemoryBlock.get() + WRAM_OFFSET;

		case VRAM:
			return sMemoryBlock.get() + VRAM_OFFSET;

		case OAM:
			return sMemoryBlock.get() + OAM_OFFSET;

		case ROM:
			return sMemoryBlock.get() + ROM_OFFSET;

		case CART_RAM:
			return sMemoryBlock.get() + CART_RAM_OFFSET;

		default:
			Emu::Assert(false, "Invalid mem block type!");
		}

		return nullptr;
	}

	Byte* MemoryMapper::GetInternal(Type blockType, USize address)
	{
		return GetBlock(blockType) + address;
	}

}
module Gameboy.Memory.AddressBus;

import Gameboy.Memory.MemoryMapper;
import Gameboy.Graphics.Flags;

namespace GB {

	void AddressBus::Init(Gameboy* ctx)
	{
		sContext = ctx;
		MemoryMapper::Init();
	}

	void AddressBus::Shutdown()
	{
		MemoryMapper::Shutdown();
		sContext = nullptr;
	}

	Byte& AddressBus::Read(Word address)
	{
		if (address >= 0xFEA0 && address < 0xFF00)
		{
			Emu::Log::Trace("Reading from restricted memory!");
			return sErrorByte;
		}

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
			Emu::Log::Warn(std::format("Reading from undocumented IO Register: {:X}", address));
			return sErrorByte;
		}

		return MemoryMapper::Read(address);
	}

	void AddressBus::Write(Word address, Byte data)
	{
		if (address >= 0xFEA0 && address < 0xFF00)
			return; //Restricted memory

		Emu::Assert(!sBootstrap || address >= 0x0100, "Cannot write to boot ROM except to disable it!");

		// Disable boot ROM
		if (sBootstrap && address == 0xFF50)
			sBootstrap = false;

		if (address < 0x8000)
			sContext->writeROM(address, data);
		else if ((address >= 0xFF00 && address < 0xFF80)) // IO Registers
			return WriteIO(address, data);

		MemoryMapper::Write(address, data);
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
			Emu::Log::Warn(std::format("Reading from undocumented IO Register: {:X}", address));
			return sErrorByte;

		default:
			return MemoryMapper::Read(address);
		}
	}

	void AddressBus::WriteIO(Word address, Byte data)
	{
		switch (address)
		{
		case Addr::DIV:
			MemoryMapper::Write(address, 0);
			break;

		case Addr::TMC:
		{
			ByteBits bits = data;
			if (bits.test(1))
				sContext->startTimer();
			else
				sContext->stopTimer();

			sContext->setTimerFreq(data & 0x3);
			break;
		}

		case Addr::LCDC:
		{
			Byte& lcdc = MemoryMapper::Read(Addr::LCDC);

			ByteBits lcdControl = lcdc;
			bool lcdPrevEnabled = lcdControl.test(LCD_ENABLE_BIT);

			lcdc = data;
			bool lcdEnabled = ByteBits(lcdc).test(LCD_ENABLE_BIT);

			if (!lcdPrevEnabled || lcdEnabled)
				break;

			sContext->disableLCD();
			break;
		}

		case Addr::STAT:
		{
			Byte& lcdStatus = MemoryMapper::Read(Addr::STAT);
			lcdStatus = (data & 0xF8) | (lcdStatus & 0x07);
			break;
		}

		case Addr::LY:
			MemoryMapper::Write(address, 0);
			break;

		case Addr::DMA:
			DoDMATransfer(data);
			break;

		case Addr::SC:
		{
			if (data == 0x81)
				Emu::Log::Info(std::format("Serial data {}", MemoryMapper::Read(Addr::SB)));
			break;
		}

			// Undocumented IO registers
		case 0xFF08: case 0xFF09: case 0xFF27: case 0xFF28: case 0xFF29:
		case 0xFF4C: case 0xFF4D: case 0xFF4E: case 0xFF56: case 0xFF57:
		case 0xFF58: case 0xFF59: case 0xFF5A: case 0xFF5B: case 0xFF5C:
		case 0xFF5D: case 0xFF5E: case 0xFF5F: case 0xFF60: case 0xFF61:
		case 0xFF62: case 0xFF63: case 0xFF64: case 0xFF65: case 0xFF66:
		case 0xFF67: case 0xFF71: case 0xFF72: case 0xFF73: case 0xFF74:
		case 0xFF75: case 0xFF78: case 0xFF79:
			Emu::Log::Error(std::format("Writing to undocumented IO Register: {:X}", address));
			break;

		default: // Default behaviour to just write to memory.
			MemoryMapper::Write(address, data);
			break;
		}
	}

	Sprite AddressBus::ReadSprite(Word address)
	{
		Emu::Assert(address >= 0xFE00 || address < 0xFEA0, "Not sprite memory!");

		return *(Sprite*)&MemoryMapper::Read(address);
	}

	Byte* AddressBus::ReadROM()
	{
		return MemoryMapper::ReadROM();
	}

	void AddressBus::RequestInterrupt(Interrupt interrupt)
	{
		Byte& req = Read(Addr::IF);
		ByteBits bits = req;
		bits.set((Byte)interrupt);
		req = (Byte)bits.to_ulong();
	}

	void AddressBus::DoDMATransfer(Byte data)
	{
		Word address = data << 8;
		Byte* src = &Read(address);
		Byte* dest = &MemoryMapper::Read(0xFE00);
		memcpy(dest, src, 0xA0);
	}
}
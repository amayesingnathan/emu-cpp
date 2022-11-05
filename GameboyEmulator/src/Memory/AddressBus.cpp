#include "gbpch.h"
#include "AddressBus.h"

#include "Address.h"

namespace GB {

	Byte AddressBus::Read(Word address)
	{
		return Byte();
	}

	void AddressBus::Write(Word address, Byte data)
	{
        GB_ASSERT(address < 0xFEA0 || address > 0xFF00, "Restricted memory!");

		if (address < 0x8000)
			;
		else if (address >= 0xA000 && address < 0xC000)
			;
		else if (address >= 0xE000 && address < 0xFE00)
			;
		else if (address == Addr::TMC)
			;
		else if (address == Addr::DIV)
			;
		else
			;
	}
}
#pragma once

#include "Memory/Memory.h"
#include "Memory/AddressBus.h"

namespace GB {

	class BootROM : public Memory
	{
	public:
		GB_CONST USize _BootROMSize = 0x0100;

	public:
		BootROM()
			: Memory(BOOTSTRAP)
		{
			std::ifstream bootROM("BootROM.gb", std::ios::binary);
			bootROM.seekg(0, std::ios::end);

			USize cartridgeSize = bootROM.tellg();
			GB_ASSERT(cartridgeSize == _BootROMSize, "Error! Boot ROM is not 256B");

			bootROM.read((char*)mBootROM, _BootROMSize);
		}

	protected:
		constexpr USize GetSize() const override { return _BootROMSize; }
		Byte& GetMemBlock(Word address) override { return mBootROM[address]; }

	private:
		Byte mBootROM[_BootROMSize];
	};

}
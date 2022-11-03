#pragma once

#include "Cartridge.h"

namespace GB {

	class CartridgeManager
	{
	public:
		static void Load(std::string_view filename) { delete sCartridge; sCartridge = new Cartridge(filename); }
		static void Shutdown() { delete sCartridge; sCartridge = nullptr; }

		static Byte Read(Word address) { GB_ASSERT(sCartridge); return sCartridge->read(address); }

		static Cartridge::BankMode BankMode() { GB_ASSERT(sCartridge); return sCartridge->getBankMode(); }
		static void ChangeLoROMBank(Byte data) { GB_ASSERT(sCartridge); sCartridge->changeLoROMBank(data); }
		static void ChangeHiROMBank(Byte data) { GB_ASSERT(sCartridge); sCartridge->changeHiROMBank(data); }

	private:
		inline static Cartridge* sCartridge = nullptr;
	};
}
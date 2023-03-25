module Gameboy.Cartridge.MBC;

import Gameboy.Memory.MemoryMapper;

namespace GB {

	void MBC1::mapWrite(Word address, Byte data)
	{
		if (address >= 0x8000)
			return;

		HandleBanking(address, data);
	}

	void MBC1::HandleBanking(Word address, Byte data)
	{
		if (address < 0x2000)
			EnableRAM(data);
		else if (address >= 0x2000 && address < 0x4000)
			ChangeROMBankLo(data);
		else if (address >= 0x4000 && address < 0x6000)
		{
			if (mUseROMBanking)
				ChangeROMBankHi(data);
			else
				ChangeRAMBank(data);
		}
		else if (address >= 0x6000 && address < 0x8000)
			ChangeROMRAMMode(data);
	}

	void MBC1::EnableRAM(Byte data)
	{
		Byte lowNibble = data & 0x0F;
		if (lowNibble == 0)
			mEnabledRAM = false;
		else if (lowNibble == 0x0A)
			mEnabledRAM = true;
	}

	void MBC1::ChangeROMBankLo(Byte data)
	{
		mROMBank = (data & 0x1F) | 0xE0;
		if (mROMBank == 0)
			mROMBank = 1;

		MemoryMapper::UpdateROMMap(mROMBank * 0x4000);
	}

	void MBC1::ChangeROMBankHi(Byte data)
	{
		mROMBank = (data & 0xE0) | 0x1F;
		if (mROMBank == 0)
			mROMBank = 1;

		MemoryMapper::UpdateROMMap(mROMBank * 0x4000);
	}

	void MBC1::ChangeRAMBank(Byte data)
	{
		mRAMBank = data & 0x03;
		MemoryMapper::UpdateRAMMap(mRAMBank * 0x2000);
	}

	void MBC1::ChangeROMRAMMode(Byte data)
	{
		ByteBits dataBits = data;
		mUseROMBanking = (dataBits.test(0));

		if (!mUseROMBanking)
			return;

		mRAMBank = 0;
		MemoryMapper::UpdateRAMMap(mRAMBank * 0x2000);
	}


	void MBC2::mapWrite(Word address, Byte data)
	{
	}
}
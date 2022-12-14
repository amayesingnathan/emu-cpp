#include "gbpch.h"
#include "MBC.h"

namespace GB {

	Word MBC1::mapRead(Word address)
	{
		if (address < 0x4000)
			return address;
		else if (address < 0x8000)
			return (address - 0x4000) + (mROMBank * 0x4000);
		else if (address >= 0xA000 && address < 0xC000)
			return (address - 0xA000) * (mRAMBank * 0x2000);

		return address;
	}

	Word MBC1::mapWrite(Word address, Byte data)
	{
		if (address < 0x8000)
		{
			HandleBanking(address, data);
			return 0;
		}
		else if (address >= 0xA000 && address < 0xC000)
		{
			if (!mEnabledRAM)
				return 0;
			return (address - 0xA000) * (mRAMBank * 0x2000);
		}
		return 0;
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
	}

	void MBC1::ChangeROMBankHi(Byte data)
	{
		mROMBank = (data & 0xE0) | 0x1F;
		if (mROMBank == 0)
			mROMBank = 1;
	}

	void MBC1::ChangeRAMBank(Byte data)
	{
		mRAMBank = data & 0x03;
	}

	void MBC1::ChangeROMRAMMode(Byte data)
	{
		BitField dataBits= data;
		mUseROMBanking = (dataBits.bit(0));
		if (mUseROMBanking)
			mRAMBank = 0;
	}


	Word MBC2::mapRead(Word address)
	{
		return Word();
	}

	Word MBC2::mapWrite(Word address, Byte data)
	{
		return Word();
	}
}
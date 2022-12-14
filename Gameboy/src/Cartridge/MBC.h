#pragma once

#include "Core/Types.h"

namespace GB {

	class MBC
	{
	public:
		virtual ~MBC() {}

		virtual Word mapRead(Word address) = 0;
		virtual Word mapWrite(Word address, Byte data) = 0;
	};

	class MBC1 : public MBC
	{
	public:
		Word mapRead(Word address) override;
		Word mapWrite(Word address, Byte data) override;

	private:
		void HandleBanking(Word address, Byte data);

		void EnableRAM(Byte data);
		void ChangeROMBankLo(Byte data);
		void ChangeROMBankHi(Byte data);
		void ChangeRAMBank(Byte data);
		void ChangeROMRAMMode(Byte data);

	private:
		Byte mROMBank = 0;
		Byte mRAMBank = 1;

		bool mEnabledRAM = false;
		bool mUseROMBanking = false;
	};

	class MBC2 : public MBC
	{
	public:
		Word mapRead(Word address) override;
		Word mapWrite(Word address, Byte data) override;
	};
}
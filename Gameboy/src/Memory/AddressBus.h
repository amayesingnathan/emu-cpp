#pragma once

#include "CPU/Interrupts.h"

namespace GB {

	struct Sprite;
	class Cartridge;

	struct BusState
	{
		bool bootstrap = true;
	};

	class AddressBus
	{
	public:
		static Byte& Read(Word address);
		static void Write(Word address, Byte data);

		static const Sprite& ReadSprite(Word address);

		static void RequestInterrupt(Interrupt interrupt);

	private:
		static void Init(Cartridge* cartridge, Word& clockSpeed) { sCartridge = cartridge; sClockSpeed = &clockSpeed; }

		static Byte& ReadIO(Word address);
		static void WriteIO(Word address, Byte data);

		static void SetClockFreq(Byte freq);
		static void DoDMATransfer(Byte data);

	private:
		inline static Cartridge* sCartridge = nullptr;
		inline static BusState sBusState;
		inline static Byte sErrorByte = 0xFF;
		inline static Word* sClockSpeed = nullptr;

		friend class Gameboy;
	};
}
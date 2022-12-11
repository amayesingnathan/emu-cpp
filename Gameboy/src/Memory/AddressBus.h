#pragma once

#include "CPU/Interrupts.h"

namespace GB {

	struct Sprite;
	class CPU;
	class PPU;
	class Cartridge;

	struct BusState
	{
		bool bootstrap = true;
	};

	class AddressBus
	{
	public:
		static void Init(CPU* cpu, PPU* ppu, Cartridge* cartridge);
		static void Shutdown();

		static Byte& Read(Word address);
		static void Write(Word address, Byte data);

		static const Sprite& ReadSprite(Word address);

		static void RequestInterrupt(Interrupt interrupt);

	private:
		static Byte& ReadIO(Word address);
		static void WriteIO(Word address, Byte data);

		static void SetClockFreq(Byte freq);
		static void DoDMATransfer(Byte data);

		static AddressBus& Get()
		{
			static AddressBus sAddressBus;
			return sAddressBus;
		}

	private:
		void init(CPU* cpu, PPU* ppu, Cartridge* cartridge) { mCPU = cpu; mPPU = ppu; mCartridge = cartridge; }
		void shutdown() { mCPU = nullptr; mPPU = nullptr; mCartridge = nullptr; }

	private:
		CPU* mCPU = nullptr;
		PPU* mPPU = nullptr;
		Cartridge* mCartridge = nullptr;
		BusState mBusState;
		Byte mErrorByte = 0xFF;

		inline static bool sInitialised = false;

		friend class Gameboy;
	};
}
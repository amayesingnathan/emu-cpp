export module Gameboy.Memory.AddressBus;

import <array>;
import Gameboy;
import Gameboy.CPU.Interrupts;
import Gameboy.Graphics.Sprite;
import Gameboy.Core.Types;

export import Gameboy.Memory.Address;

namespace GB {

	export class AddressBus
	{
	public:
		static void Init(Gameboy* ctx);
		static void Shutdown();

		static Byte& Read(Word address);
		static void Write(Word address, Byte data);

		static Sprite ReadSprite(Word address);
		static Byte* ReadROM();

		static void RequestInterrupt(Interrupt interrupt);
		static void DoDMATransfer(Byte data);

	private:
		static Byte& ReadIO(Word address);
		static void WriteIO(Word address, Byte data);

		static void PostBoot();

	private:
		inline static Gameboy* sContext = nullptr;
		inline static Byte sErrorByte = 0xFF;
		inline static bool sBootstrap = true;
	};
}
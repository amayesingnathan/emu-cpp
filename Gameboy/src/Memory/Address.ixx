export module Gameboy.Memory.Address;

import Gameboy.Core.Types;

export namespace GB {

	namespace Addr {

		constexpr Word JOYP = 0xFF00;

		constexpr Word DIV = 0xFF04;
		constexpr Word TIMA = 0xFF05;
		constexpr Word TMA = 0xFF06;
		constexpr Word TMC = 0xFF07;

		constexpr Word LCDC = 0xFF40;
		constexpr Word STAT = 0xFF41;
		constexpr Word SCY = 0xFF42;
		constexpr Word SCX = 0xFF43;
		constexpr Word LY = 0xFF44;
		constexpr Word LYC = 0xFF45;
		constexpr Word DMA = 0xFF46;
		constexpr Word BGP = 0xFF47;
		constexpr Word OBP0 = 0xFF48;
		constexpr Word OBP1 = 0xFF49;

		constexpr Word WY = 0xFF4A;
		constexpr Word WX = 0xFF4B;

		constexpr Word IF = 0xFF0F;
		constexpr Word IE = 0xFFFF;
	}

}
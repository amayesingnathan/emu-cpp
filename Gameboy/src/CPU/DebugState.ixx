export module DebugState;

import Gameboy.Core.Types;

export namespace GB {

	struct InternalState
	{
		Byte& LCDC;
		Byte& STAT;
		Byte& SCY;
		Byte& SCX;
		Byte& LY;
		Byte& LYC;

		Byte& DMA;
		Byte& BGP;
		Byte& OBP0;
		Byte& OBP1;

		Byte& WY;
		Byte& WX;

		Byte& JOYP;
		Byte& SB;
		Byte& SC;

		Byte& DIV;
		Byte& TIMA;
		Byte& TMA;
		Byte& TMC;

		Byte& IF;
		Byte& IE;

		InternalState();
	};
}
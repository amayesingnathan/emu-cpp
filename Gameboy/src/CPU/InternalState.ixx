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

		bool Paused = true;
		bool Break = false;
		Word Breakpoint = 0x100;
		USize LastFrameCount = 0;

		InternalState();
	};
}
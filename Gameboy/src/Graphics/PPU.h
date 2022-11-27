#pragma once

#include "Core/Types.h"

namespace GB {

	struct Sprite
	{
		Byte xPos;
		Byte yPos;
		Byte tileLoc;
		BitField flags;
	};

	class PPU
	{
	private:
		GB_CONST Byte _ScreenWidth  = 160;
		GB_CONST Byte _ScreenHeight = 144;

		GB_CONST Word _CyclesPerScanline = 456;
		GB_CONST Byte _ScanlineCount	 = 144;
		GB_CONST Byte _ScanlineCountMax	 = 154;

		GB_CONST Byte _HBlankCycles		 = 204;
		GB_CONST Byte _OAMCycles		 = 80;
		GB_CONST Byte _LCDTransferCycles = 172;

		GB_CONST Emu::Pixel _White	   = { 0xFF, 0xFF, 0xFF, 0xFF };
		GB_CONST Emu::Pixel _LightGrey = { 0xCC, 0xCC, 0xCC, 0xFF };
		GB_CONST Emu::Pixel _DarkGrey  = { 0x77, 0x77, 0x77, 0xFF };
		GB_CONST Emu::Pixel _Black	   = { 0x00, 0x00, 0x00, 0xFF };

		enum Mode : Byte { HBLANK = 0, VBLANK, OAM, LCD_TR };

		enum Colour : Byte { WHITE, LIGHT_GREY, DARK_GREY, BLACK };

	public:
		PPU();
		~PPU();

	public:
		void startFrame() { mDisplayPB->lock(); }
		void endFrame() { mDisplayPB->unlock(); }

		void update(Byte cycles);

		Emu::uint getDisplayTex() const { return mDisplayPB->getTexID(); }

	private:
		bool IsLCDEnabled();
		void DisableLCD();
		void SetLCDStatus(Mode newMode);
		void CompareScanline(Byte scanline);

		void HBlankMode();
		void VBlankMode();
		void OAMMode();
		void LCDTransferMode();

		void DrawScanline();
		void DrawTiles(BitField lcdControl);
		void DrawSprites(BitField lcdControl);

		Colour GetColour(Byte colourNum, Word address);

	private:
		Ref<Emu::PixelBuffer> mDisplayPB, mSpritesPB;

		Word mClockCounter = 0;
		Mode mCurrentMode = OAM;

		friend class Gameboy;
	};
}
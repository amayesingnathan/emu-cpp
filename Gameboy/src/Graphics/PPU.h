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
		GB_CONST Byte SCREEN_WIDTH  = 160;
		GB_CONST Byte SCREEN_HEIGHT = 144;

		GB_CONST Word CYCLES_PER_SCANLINE  = 456;
		GB_CONST Byte SCANLINE_COUNT	   = 144;
		GB_CONST Byte SCANLINE_COUNT_MAX   = 154;

		GB_CONST Byte HBLANK_CYCLES		   = 204;
		GB_CONST Byte OAM_CYCLES		   = 80;
		GB_CONST Byte LCD_TRANSFER_CYCLES  = 172;

		GB_CONST Emu::Pixel WHITE	   = { 0xFF, 0xFF, 0xFF, 0xFF };
		GB_CONST Emu::Pixel LIGHT_GREY = { 0xCC, 0xCC, 0xCC, 0xFF };
		GB_CONST Emu::Pixel DARK_GREY  = { 0x77, 0x77, 0x77, 0xFF };
		GB_CONST Emu::Pixel BLACK	   = { 0x00, 0x00, 0x00, 0xFF };

		enum class Mode : Byte { HBLANK = 0, VBLANK, OAM, LCD_TR };

		enum class Colour : Byte { WHITE, LIGHT_GREY, DARK_GREY, BLACK };

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
		Mode mCurrentMode = Mode::OAM;

		friend class Gameboy;
	};
}
#pragma once

#include "Graphics/Framebuffer.h"
#include "Graphics/Texture.h"

#include "Screen.h"

namespace GB {

	class PPU
	{
	public:
		GB_CONST USize _BufSize = Screen::_Width * Screen::_Height * sizeof(Emu::Pixel);

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
		void startFrame();// { mDisplay = mPixelBuffer->lock(); }
		void endFrame();// { mPixelBuffer->upload(mDisplayTexture); }

		void update(Byte cycles);

		Emu::uint getDisplayTex() const { return mFramebuffer->getColourAttachment(); }

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
		Ref<Emu::Framebuffer> mFramebuffer;
		//Ref<Emu::Texture> mDisplayTexture;
		Screen mDisplay;

		Word mClockCounter = 0;
		Mode mCurrentMode = OAM;

		friend class Gameboy;
	};
}
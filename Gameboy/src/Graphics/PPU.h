#pragma once

#include "Graphics/PixelBuffer.h"
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

		enum Mode : Byte
		{
			HBLANK = 0, VBLANK, OAM, LCD_TR
		};

	public:
		PPU();
		~PPU();

	public:
		void startFrame() { mDisplay = mPixelBuffer->lock(); }
		void endFrame() { mPixelBuffer->upload(mDisplayTexture); }

		void update(Byte cycles);

		Emu::uint getDisplayTex() const { return mDisplayTexture->getTexID(); }

	private:
		bool IsLCDEnabled();

		void HBlankMode();
		void VBlankMode();
		void OAMMode();
		void LCDTransferMode();

		void DrawSprites();

	private:
		Ref<Emu::PixelBuffer> mPixelBuffer;
		Ref<Emu::Texture> mDisplayTexture;
		Screen mDisplay;

		Word mClockCounter = 0;
		Mode mCurrentMode = OAM;
	};
}
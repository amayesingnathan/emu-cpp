export module Gameboy.Graphics;

import Gameboy.Core.Types;
export import Gameboy.Graphics.Flags;

import EmuBase.Graphics.PixelBuffer;

export namespace GB {

	class PPU
	{
	private:
		static constexpr Byte SCREEN_WIDTH = 160;
		static constexpr Byte SCREEN_HEIGHT = 144;

		static constexpr Word CYCLES_PER_SCANLINE = 456;
		static constexpr Byte SCANLINE_COUNT = 144;
		static constexpr Byte SCANLINE_COUNT_MAX = 154;

		static constexpr Byte HBLANK_CYCLES = 204;
		static constexpr Byte OAM_CYCLES = 80;
		static constexpr Byte LCD_TRANSFER_CYCLES = 172;

		static constexpr Emu::Pixel WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
		static constexpr Emu::Pixel LIGHT_GREY = { 0xCC, 0xCC, 0xCC, 0xFF };
		static constexpr Emu::Pixel DARK_GREY = { 0x77, 0x77, 0x77, 0xFF };
		static constexpr Emu::Pixel BLACK = { 0x00, 0x00, 0x00, 0xFF };
		static constexpr Emu::Pixel RED = { 0xFF, 0x00, 0x00, 0xFF };

		enum class Mode : Byte { HBLANK = 0, VBLANK, OAM, LCD_TR };

		enum class Colour : Byte { WHITE, LIGHT_GREY, DARK_GREY, BLACK };

	public:
		PPU();

	public:
		void startFrame() { mDisplayPB->lock(); }
		void endFrame() { mDisplayPB->unlock(); }

		void tick(Byte cycles);

		Emu::uint getDisplayTex() const { return mDisplayPB->getTexID(); }

		void disableLCD();

	private:
		void SetLCDStatus(Mode newMode);
		void CompareScanline();

		void HBlankMode();
		void VBlankMode();
		void OAMMode();
		void LCDTransferMode();

		void DrawScanline();
		void DrawTiles();
		void DrawSprites();

		Colour GetColour(Byte colourNum, Word address);

	private:
		Emu::Ref<Emu::PixelBuffer> mDisplayPB, mSpritesPB;

		Word mClockCounter = 0;
		Mode mCurrentMode = Mode::OAM;

		Byte& mLCDScanline;
		Byte& mLCDComparison;
		Byte& mLCDStatus;
		Byte& mLCDControl;

		friend class Gameboy;
		friend class AddressBus;
	};
}
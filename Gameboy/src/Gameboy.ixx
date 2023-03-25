export module Gameboy;

export import EmuBase.Emulator;

import Gameboy.Core.Types;

namespace GB {

	class CPU;
	class PPU;
	class Cartridge;

	struct APU {};

	export class Gameboy : public Emu::Base
	{
	public:
		static constexpr USize CYCLES_PER_FRAME = 70224;

	public:
		Gameboy(std::string_view path);
		~Gameboy();

		Gameboy(const Gameboy&) = delete;
		Gameboy& operator=(const Gameboy&) = delete;

		void update() override;
		void uiRender() override;
		void onEvent(Emu::Event& event) override;

		Emu::uint getDisplayTex() override;
		Emu::Duration getFrameTime() override;

		void setTimerFreq(Byte freq);
		void startTimer();
		void stopTimer();

		void disableLCD();

		void writeROM(Word addr, Byte val);

	private:
		void UI_CPU();
		void UI_Memory();

	private:
		Emu::Impl<Cartridge> mCartridge = nullptr;

		Emu::Impl<CPU> mProcessor = nullptr;
		Emu::Impl<PPU> mGraphics = nullptr;
		Emu::Impl<APU> mAudio = nullptr;
	};
}
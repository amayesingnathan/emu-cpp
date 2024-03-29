export module Gameboy;

import <sstream>;

export import EmuBase.Emulator;

import Gameboy.Core.Types;
import DebugState;

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
		void Update();
		void UpdateDebug();
		void UpdateOnce();

		void UI_CPU();
		void UI_Debugger();
		void UI_Memory();

	private:
		Emu::Impl<Cartridge> mCartridge = nullptr;

		Emu::Impl<CPU> mProcessor = nullptr;
		Emu::Impl<PPU> mGraphics = nullptr;
		Emu::Impl<APU> mAudio = nullptr;

		Emu::Impl<InternalState> mDebugState;
	};
}
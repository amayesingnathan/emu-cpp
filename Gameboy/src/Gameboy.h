#pragma once

#include "CPU/CPU.h"
#include "Graphics/PPU.h"
#include "Cartridge/Cartridge.h"
#include "IO/Actions.h"

namespace GB {

    struct APU {};

    struct GBSession
    {
        USize lastCycles = 0;

        bool paused = true;
        bool step = false;

        Word lastPC = 0x0000;
        Word breakpoint = 0x0100;
        bool useBreakpoint = false;

        bool print = true;
    };

    class Gameboy : public Emu::Base
    {
    public:
        GB_CONST USize          CYCLES_PER_FRAME    = 70224;
        GB_CONST Emu::Duration  FRAME_LENGTH        = Emu::Duration((float)(CYCLES_PER_FRAME * 1000) / (float)CPU::CLOCK_SPEED);

    public:
        Gameboy(Emu::Window* window, std::string_view path);
        ~Gameboy() override;

    public:
        void update() override;
        void imguiRender() override;
        void onEvent(Emu::Event& event) override;

        Emu::uint getDisplayTex() override { return mGraphics->getDisplayTex(); }
        Emu::Duration getFrameTime() override { return FRAME_LENGTH; };

    private:
        void UI_CPU();

        bool KeyPressedEvent(Emu::KeyPressedEvent& event);

    private:
        Cartridge* mCartridge = nullptr;

        CPU* mProcessor = nullptr;
        PPU* mGraphics = nullptr;
        APU* mAudio = nullptr;

        Emu::ImGuiHandler* mImGuiHandler = nullptr;
        GBSession* mSession = nullptr;
    };
}
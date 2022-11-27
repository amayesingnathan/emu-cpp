#pragma once

#include "CPU/CPU.h"
#include "Graphics/PPU.h"
#include "Cartridge/Cartridge.h"
#include "IO/Actions.h"

namespace GB {

    struct APU {};

    class Gameboy : public Emu::Base
    {
    public:
        GB_CONST USize _ClockSpeed          = 4194304;
        GB_CONST USize _CyclesPerFrame      = 70224;
        GB_CONST Emu::Duration _FrameLength = Emu::Duration((float)(_CyclesPerFrame * 1000) / (float)_ClockSpeed);

    public:
        Gameboy(Emu::Window* window, std::string_view path);
        ~Gameboy() override;

    public:
        void update() override;
        void imguiRender() override;

        Emu::ActionCallback getActionCallback() override { return BIND_ACTION_FUNC(Gameboy::HandleEvent); };
        Emu::uint getDisplayTex() override { return mGraphics.getDisplayTex(); }
        Emu::Duration getFrameTime() override { return _FrameLength; };

    private:
        void HandleEvent(Emu::Action action, bool pressed);

        void UI_CPU();

    private:
        Cartridge* mCartridge = nullptr;

        CPU mProcessor;
        PPU mGraphics;
        APU mAudio;

        Emu::ImGuiHandler* mImGuiHandler = nullptr;

        USize mLastCycles = 0;

        // Debug
        bool mPaused = false;
        bool mStep = false;

        Word mPCBreakpoint = 0x0100;
        bool mUseBreakpoint = false;
    };
}
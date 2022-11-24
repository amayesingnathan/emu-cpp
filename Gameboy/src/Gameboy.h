#pragma once

#include "EmuBase.h"

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
        GB_CONST Emu::Duration _FrameLength = Emu::Duration((float)_CyclesPerFrame / (float)_ClockSpeed);

    public:
        Gameboy(std::string_view path);
        ~Gameboy() override;

    public:
        void update() override;

        Emu::ActionCallback getActionCallback() { return BIND_ACTION_FUNC(Gameboy::HandleEvent); };
        Emu::uint getDisplayTex() override { return mGraphics.getDisplayTex(); };
        Emu::Duration getFrameTime() override { return _FrameLength; };

    private:
        void UpdateGraphics(int cycles)
        {
        }

        void Interupts()
        {
        }

        void Render()
        {

        }

        void HandleEvent(Emu::Action action, bool pressed)
        {
            switch (action)
            {
            case Buttons::Up: break;
            case Buttons::Down: break;
            case Buttons::Left: break;
            case Buttons::Right: break;
            case Buttons::A: break;
            case Buttons::B: break;
            case Buttons::Start: break;
            case Buttons::Select: break;
            default: GB_ASSERT(false);
            }
        }

    private:
        Cartridge* mCartridge = nullptr;

        CPU mProcessor;
        PPU mGraphics;
        APU mAudio;
    };
}
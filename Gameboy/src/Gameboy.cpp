#include "gbpch.h"
#include "Gameboy.h"

#include "imgui.h"

#include "Memory/MemoryManager.h"

namespace GB {

	Gameboy::Gameboy(Emu::Window* window, std::string_view path)
    {
        mSession = new GBSession;
        mProcessor = new CPU(mSession);
        mGraphics = new PPU;

        MemoryManager::Init();
        mCartridge = new Cartridge(path);
        AddressBus::Init(mProcessor, mGraphics, mCartridge);
    }

    Gameboy::~Gameboy()
    {
        delete mCartridge;
        delete mGraphics;
        delete mProcessor;
        delete mSession;

        AddressBus::Shutdown();
        MemoryManager::Shutdown();
    }

    void Gameboy::update()
    {
        if (mSession->paused && !mSession->step)
            return;

        if (mSession->lastCycles == 0)
            mGraphics->startFrame();

        USize cyclesThisUpdate = mSession->lastCycles;
        mSession->lastCycles = 0;

        while (cyclesThisUpdate < CYCLES_PER_FRAME)
        {
            Byte cycles = mProcessor->exec();

            if (cycles == 0)
                break;

            cyclesThisUpdate += cycles;
            mProcessor->updateTimers(cycles);
            mGraphics->update(cycles);
            mProcessor->handleInterupts();

            if (mSession->step)
                break;
        }

        if (mSession->step)
        {
            mSession->lastCycles = cyclesThisUpdate;
            mSession->step = false;
        }

        if (mSession->lastCycles == 0)
            mGraphics->endFrame();
    }

    void Gameboy::imguiRender()
    {
        UI_CPU();
    }

    void Gameboy::onEvent(Emu::Event& event)
    {
        Emu::EventDispatcher dispatcher(event);
        dispatcher.dispatch<Emu::KeyPressedEvent>(BIND_EVENT_FUNC(Gameboy::KeyPressedEvent));
    }

    void Gameboy::UI_CPU()
    {
        ImGui::Begin("CPU");

        {   // Control
            const char* label = mSession->paused ? "Run" : "Pause";
            if (ImGui::Button(label))
                mSession->paused = !mSession->paused;

            if (mSession->paused)
            {
                ImGui::SameLine();
                if (ImGui::Button("Step"))
                    mSession->step = true;
            }

            ImGui::SameLine();
            ImGui::Checkbox("Breakpoint", &mSession->useBreakpoint);

            static Word sStepSize = 1;
            ImGui::InputScalar("PC", ImGuiDataType_U16, &mSession->breakpoint, &sStepSize, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);

            ImGui::Checkbox("Log", &mSession->print);
        }

        ImGui::Separator();

        {   // Registers
            Registers& registers = mProcessor->mRegisters;
            BitField flagReg = mProcessor->mFRegister.getFlags();

            ImGui::Text("CPU Registers");

            ImGui::Text("A: %02X", registers[ByteReg::A]);
            ImGui::SameLine();
            ImGui::Text("F: %02X", registers[ByteReg::F]);
            ImGui::SameLine();
            ImGui::Text("AF: %04X", registers[WordReg::AF]);

            ImGui::Text("Flag Register");
            ImGui::Text("Zero : %1i", flagReg.val(ZERO_BIT));
            ImGui::SameLine();
            ImGui::Text("Sub : %1i", flagReg.val(SUBTRACT_BIT));
            ImGui::SameLine();
            ImGui::Text("HCarry : %1i", flagReg.val(H_CARRY_BIT));
            ImGui::SameLine();
            ImGui::Text("Carry : %1i", flagReg.val(CARRY_BIT));

            ImGui::Text("B: %02X", registers[ByteReg::B]);
            ImGui::SameLine();
            ImGui::Text("C: %02X", registers[ByteReg::C]);
            ImGui::SameLine();
            ImGui::Text("BC: %04X", registers[WordReg::BC]);

            ImGui::Text("D: %02X", registers[ByteReg::D]);
            ImGui::SameLine();
            ImGui::Text("E: %02X", registers[ByteReg::E]);
            ImGui::SameLine();
            ImGui::Text("DE: %04X", registers[WordReg::DE]);

            ImGui::Text("H: %02X", registers[ByteReg::H]);
            ImGui::SameLine();
            ImGui::Text("L: %02X", registers[ByteReg::L]);
            ImGui::SameLine();
            ImGui::Text("HL: %04X", registers[WordReg::HL]);

            ImGui::Text("SP: %04X", registers[WordReg::SP]);

            ImGui::Text("PC: %04X", registers[WordReg::PC]);
        }

        ImGui::Separator();

        {   // Registers
            Byte* ioRegisters = MemoryManager::GetBlock(MemoryManager::IO);
            BitField IE = ioRegisters[0x80];

            ImGui::Text("IO Registers");

            ImGui::Text("JOYP: %02X", ioRegisters[0x0]);

            ImGui::Text("SB: 0x%02X", ioRegisters[0x1]);
            ImGui::SameLine();
            ImGui::Text("SC: 0x%02X", ioRegisters[0x2]);

            ImGui::Text("DIV: 0x%02X", ioRegisters[0x4]);
            ImGui::SameLine();
            ImGui::Text("TIMA: 0x%02X", ioRegisters[0x5]);
            ImGui::SameLine();
            ImGui::Text("TMA: 0x%02X", ioRegisters[0x6]);
            ImGui::SameLine();
            ImGui::Text("TMC: 0x%02X", ioRegisters[0x7]);

            ImGui::Text("LCDC: 0x%02X", ioRegisters[0x40]);
            ImGui::SameLine();
            ImGui::Text("LCDS: 0x%02X", ioRegisters[0x41]);

            ImGui::Text("SCY: 0x%02X", ioRegisters[0x42]);
            ImGui::SameLine();
            ImGui::Text("SCX: 0x%02X", ioRegisters[0x43]);

            ImGui::Text("LY: %03i", ioRegisters[0x44]);
            ImGui::SameLine();
            ImGui::Text("LYC: %03i", ioRegisters[0x45]);

            ImGui::Text("DMA: 0x%02X", ioRegisters[0x46]);

            ImGui::Text("BGP: 0x%02X", ioRegisters[0x47]);
            ImGui::SameLine();
            ImGui::Text("OPB0: 0x%02X", ioRegisters[0x48]);
            ImGui::SameLine();
            ImGui::Text("OPB1: 0x%02X", ioRegisters[0x49]);

            ImGui::Text("WY: 0x%02X", ioRegisters[0x4A]);
            ImGui::SameLine();
            ImGui::Text("WX: 0x%02X", ioRegisters[0x4B]);

            ImGui::Text("IE: 0x%02X", IE);
            ImGui::SameLine();
            ImGui::Text("IF: 0x%02X", ioRegisters[0x0F]);

            ImGui::Text("Interupt Enable Register");
            ImGui::Text("VBlank : %1i", IE.val(VBLANK_BIT));
            ImGui::SameLine();
            ImGui::Text("LCD Status : %1i", IE.val(LCD_STAT_BIT));

            ImGui::Text("Timer : %1i", IE.val(TIMER_BIT));
            ImGui::SameLine();
            ImGui::Text("Serial : %1i", IE.val(SERIAL_BIT));
            ImGui::SameLine();
            ImGui::Text("Joypad : %1i", IE.val(JOYPAD_BIT));
        }

        ImGui::End();
    }

    bool Gameboy::KeyPressedEvent(Emu::KeyPressedEvent& event)
    {
        Emu::KeyCode key = event.getKeyCode();
        if (!Emu::Input::IsValid(key))
            return false;

        Emu::Action action = Emu::Input::GetEmuButton(key);

        switch (action)
        {
        case Buttons::Up: return true;
        case Buttons::Down: return true;
        case Buttons::Left: return true;
        case Buttons::Right: return true;
        case Buttons::A: return true;
        case Buttons::B: return true;
        case Buttons::Start: return true;
        case Buttons::Select: return true;
        }

        return false;
    }
}
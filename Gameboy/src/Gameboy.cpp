#include "gbpch.h"
#include "Gameboy.h"

#include "imgui.h"

#include "Memory/MemoryManager.h"

namespace GB {

	Gameboy::Gameboy(Emu::Window* window, std::string_view path)
    {
        MemoryManager::Init();
        mCartridge = new Cartridge(path);
        AddressBus::Init(mCartridge, mProcessor.getClockSpeed());
    }

    Gameboy::~Gameboy()
    {   
        delete mCartridge;
        mCartridge = nullptr;

        MemoryManager::Shutdown();
    }

    void Gameboy::update()
    {
        if (mPaused && !mStep)
            return;

        if (mLastCycles == 0)
            mGraphics.startFrame();

        USize cyclesThisUpdate = mLastCycles;
        mLastCycles = 0;

        while (cyclesThisUpdate < _CyclesPerFrame)
        {
            Byte cycles = 0;
            if (!mUseBreakpoint)
                cycles = mProcessor.exec();
            else
                cycles = mProcessor.execDebug(mPCBreakpoint, mPaused, mStep);


            cyclesThisUpdate += cycles;
            mProcessor.updateTimers(cycles);
            mGraphics.update(cycles);
            mProcessor.handleInterupts();

            if (mStep)
                break;
        }

        if (mStep)
        {
            mLastCycles = cyclesThisUpdate;
            mStep = false;
        }

        if (mLastCycles == 0)
            mGraphics.endFrame();
    }

    void Gameboy::imguiRender()
    {
        UI_CPU();
    }

    void Gameboy::HandleEvent(Emu::Action action, bool pressed)
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

    void Gameboy::UI_CPU()
    {
        ImGui::Begin("CPU");

        {   // Control
            const char* label = mPaused ? "Run" : "Pause";
            if (ImGui::Button(label))
                mPaused = !mPaused;

            if (mPaused)
            {
                ImGui::SameLine();
                if (ImGui::Button("Step"))
                    mStep = true;
            }

            ImGui::SameLine();
            ImGui::Checkbox("Breakpoint", &mUseBreakpoint);

            static Word sStepSize = 1;
            ImGui::InputScalar("PC", ImGuiDataType_U16, &mPCBreakpoint, &sStepSize, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
        }

        ImGui::Separator();

        {   // Registers
            Registers& registers = mProcessor.mRegisters;
            BitField flagReg = mProcessor.mFRegister.getFlags();

            ImGui::Text("CPU Registers");

            ImGui::Text("A: %02X", registers[ByteReg::A]);
            ImGui::SameLine();
            ImGui::Text("F: %02X", registers[ByteReg::F]);
            ImGui::SameLine();
            ImGui::Text("AF: %04X", registers[WordReg::AF]);

            ImGui::Text("Flag Register");
            ImGui::Text("Zero : %1i", flagReg.val(_ZeroBit));
            ImGui::SameLine();
            ImGui::Text("Sub : %1i", flagReg.val(_SubtractBit));
            ImGui::SameLine();
            ImGui::Text("HCarry : %1i", flagReg.val(_HCarryBit));
            ImGui::SameLine();
            ImGui::Text("Carry : %1i", flagReg.val(_CarryBit));

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

            ImGui::Text("SB: %02X", ioRegisters[0x1]);
            ImGui::SameLine();
            ImGui::Text("SC: %02X", ioRegisters[0x2]);

            ImGui::Text("DIV: %02X", ioRegisters[0x4]);
            ImGui::SameLine();
            ImGui::Text("TIMA: %02X", ioRegisters[0x5]);
            ImGui::SameLine();
            ImGui::Text("TMA: %02X", ioRegisters[0x6]);
            ImGui::SameLine();
            ImGui::Text("TMC: %02X", ioRegisters[0x7]);

            ImGui::Text("LCDC: %02X", ioRegisters[0x40]);
            ImGui::SameLine();
            ImGui::Text("LCDS: %02X", ioRegisters[0x41]);

            ImGui::Text("SCY: %02X", ioRegisters[0x42]);
            ImGui::SameLine();
            ImGui::Text("SCX: %02X", ioRegisters[0x43]);

            ImGui::Text("LY: %03i", ioRegisters[0x44]);
            ImGui::SameLine();
            ImGui::Text("LYC: %03i", ioRegisters[0x45]);

            ImGui::Text("BGP: %02X", ioRegisters[0x47]);
            ImGui::SameLine();
            ImGui::Text("OPB0: %02X", ioRegisters[0x48]);
            ImGui::SameLine();
            ImGui::Text("OPB1: %02X", ioRegisters[0x49]);

            ImGui::Text("WY: %02X", ioRegisters[0x4A]);
            ImGui::SameLine();
            ImGui::Text("WX: %02X", ioRegisters[0x4B]);

            ImGui::Text("IE: %02X", &IE);
            ImGui::SameLine();
            ImGui::Text("IF: %02X", ioRegisters[0x0F]);

            ImGui::Text("Interupt Enable Register");
            ImGui::Text("VBlank : %1i", IE.val(_VBlankBit));
            ImGui::SameLine();
            ImGui::Text("LCD Status : %1i", IE.val(_LCDStatBit));

            ImGui::Text("Timer : %1i", IE.val(_TimerBit));
            ImGui::SameLine();
            ImGui::Text("Serial : %1i", IE.val(_SerialBit));
            ImGui::SameLine();
            ImGui::Text("Joypad : %1i", IE.val(_JoypadBit));
        }

        ImGui::End();
    }
}
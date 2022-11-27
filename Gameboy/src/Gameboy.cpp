#include "gbpch.h"
#include "Gameboy.h"

#include "imgui.h"

#include "Memory/MemoryManager.h"

namespace GB {

	Gameboy::Gameboy(Emu::Window* window, std::string_view path)
    {
        MemoryManager::Init();
        mCartridge = new Cartridge(path);
        AddressBus::Init(mCartridge);
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
            Byte cycles = mProcessor.exec();
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
        }

        ImGui::Separator();

        {   // Registers
            Registers& registers = mProcessor.mRegisters;

            ImGui::Text("A: %02X", registers[ByteReg::A]);
            ImGui::SameLine();
            ImGui::Text("F: %02X", registers[ByteReg::F]);
            ImGui::SameLine();
            ImGui::Text("AF: %04X", registers[WordReg::AF]);

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

        ImGui::End();
    }
}
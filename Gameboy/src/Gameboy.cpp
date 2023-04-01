module;

#include <imgui.h>

module Gameboy;

import <fstream>;

import Gameboy.CPU;
import Gameboy.Graphics;
import Gameboy.Cartridge;
import Gameboy.Memory.AddressBus;
import Gameboy.Memory.MemoryMapper;

namespace GB {

    Gameboy::Gameboy(std::string_view path)
    {
        AddressBus::Init(this);

        mProcessor = Emu::MakeInstance<CPU>();
        mGraphics = Emu::MakeInstance<PPU>();

        mCartridge = Emu::MakeInstance<Cartridge>(path);

        mDebugState = Emu::MakeInstance<InternalState>();
    }

    Gameboy::~Gameboy()
    {
        AddressBus::Shutdown();
    }

    void Gameboy::update()
    {
        if (mDebugState->Break)
            return UpdateDebug();

        Update();
    }

    void Gameboy::uiRender()
    {
        UI_CPU();
        UI_Debugger();
        //UI_Memory();
    }

    void Gameboy::onEvent(Emu::Event& event)
    {
    }

    Emu::uint Gameboy::getDisplayTex()
    {
        return mGraphics->getDisplayTex();
    }

    Emu::Duration Gameboy::getFrameTime()
    {
        constexpr Emu::Duration FRAME_LENGTH = Emu::Duration((float)(CYCLES_PER_FRAME * 1000) / (float)CLOCK_SPEED);

        return FRAME_LENGTH;
    }

    void Gameboy::setTimerFreq(Byte freq)
    {
        mProcessor->setTimerFreq(freq);
    }

    void Gameboy::startTimer()
    {
        mProcessor->startTimer();
    }

    void Gameboy::stopTimer()
    {
        mProcessor->stopTimer();
    }

    void Gameboy::writeROM(Word addr, Byte val)
    {
        mCartridge->write(addr, val);
    }

    void Gameboy::Update()
    {
        mGraphics->startFrame();

        USize cyclesThisUpdate = 0;
        while (!mDebugState->Paused && cyclesThisUpdate < CYCLES_PER_FRAME)
        {
            Byte cycles = mProcessor->tick();
            cyclesThisUpdate += cycles;
            mGraphics->tick(cycles);
            mProcessor->handleInterrupts();
        }

        mGraphics->endFrame();
    }

    void Gameboy::UpdateDebug()
    {
        mGraphics->startFrame();

        USize cyclesThisUpdate = mDebugState->LastFrameCount;
        while (!mDebugState->Paused && cyclesThisUpdate < CYCLES_PER_FRAME)
        {
            Byte cycles = mProcessor->tickDebug(mDebugState->Breakpoint);
            cyclesThisUpdate += cycles;
            mGraphics->tick(cycles);
            mProcessor->handleInterrupts();

            mDebugState->Paused = cycles == 0;
        }

        mDebugState->LastFrameCount = 0;
        if (cyclesThisUpdate < CYCLES_PER_FRAME)
            mDebugState->LastFrameCount = cyclesThisUpdate;

        mGraphics->endFrame();
    }

    void Gameboy::UpdateOnce()
    {
        Byte cycles = mProcessor->tick();
        mGraphics->tick(cycles);
        mProcessor->handleInterrupts();
    }

    void Gameboy::disableLCD()
    {
        mGraphics->disableLCD();
    }

    void Gameboy::UI_CPU()
    {
        ImGui::Begin("CPU");

        {   // Registers
            Registers& registers = mProcessor->mRegisters;
            FlagRegister flagReg = mProcessor->mFRegister;

            ImGui::Text("CPU Registers");

            ImGui::Text("A: %02X", registers[ByteReg::A].value());
            ImGui::SameLine();
            ImGui::Text("F: %02X", registers[ByteReg::F].value());
            ImGui::SameLine();
            ImGui::Text("AF: %04X", registers[WordReg::AF].value());

            ImGui::Text("Flag Register");
            ImGui::Text("Zero : %1i", flagReg.zero());
            ImGui::SameLine();
            ImGui::Text("Sub : %1i", flagReg.subtr());
            ImGui::SameLine();
            ImGui::Text("HCarry : %1i", flagReg.hcarry());
            ImGui::SameLine();
            ImGui::Text("Carry : %1i", flagReg.carry());

            ImGui::Text("B: %02X", registers[ByteReg::B].value());
            ImGui::SameLine();
            ImGui::Text("C: %02X", registers[ByteReg::C].value());
            ImGui::SameLine();
            ImGui::Text("BC: %04X", registers[WordReg::BC].value());

            ImGui::Text("D: %02X", registers[ByteReg::D].value());
            ImGui::SameLine();
            ImGui::Text("E: %02X", registers[ByteReg::E].value());
            ImGui::SameLine();
            ImGui::Text("DE: %04X", registers[WordReg::DE].value());

            ImGui::Text("H: %02X", registers[ByteReg::H].value());
            ImGui::SameLine();
            ImGui::Text("L: %02X", registers[ByteReg::L].value());
            ImGui::SameLine();
            ImGui::Text("HL: %04X", registers[WordReg::HL].value());

            ImGui::Text("SP: %04X", registers[WordReg::SP].value());

            ImGui::Text("PC: %04X", registers[WordReg::PC].value());
        }

        ImGui::Separator();

        {   // Registers

            ImGui::Text("IO Registers");

            ImGui::Text("JOYP: 0x%02X", mDebugState->JOYP);

            ImGui::Text("SB: 0x%02X", mDebugState->SB);
            ImGui::SameLine();
            ImGui::Text("SC: 0x%02X", mDebugState->SC);

            ImGui::Text("DIV: 0x%02X", mDebugState->DIV);
            ImGui::SameLine();
            ImGui::Text("TIMA: 0x%02X", mDebugState->JOYP);
            ImGui::SameLine();
            ImGui::Text("TMA: 0x%02X", mDebugState->TMA);
            ImGui::SameLine();
            ImGui::Text("TMC: 0x%02X", mDebugState->TMC);

            ImGui::Text("LCDC: 0x%02X", mDebugState->LCDC);
            ImGui::SameLine();
            ImGui::Text("STAT: 0x%02X", mDebugState->STAT);

            ImGui::Text("SCY: 0x%02X", mDebugState->SCY);
            ImGui::SameLine();
            ImGui::Text("SCX: 0x%02X", mDebugState->SCX);

            ImGui::Text("LY: %03i", mDebugState->LY);
            ImGui::SameLine();
            ImGui::Text("LYC: %03i", mDebugState->LYC);

            ImGui::Text("DMA: 0x%02X", mDebugState->DMA);

            ImGui::Text("BGP: 0x%02X", mDebugState->BGP);
            ImGui::SameLine();
            ImGui::Text("OBP0: 0x%02X", mDebugState->OBP0);
            ImGui::SameLine();
            ImGui::Text("OBP1: 0x%02X", mDebugState->OBP1);

            ImGui::Text("WY: 0x%02X", mDebugState->WY);
            ImGui::SameLine();
            ImGui::Text("WX: 0x%02X", mDebugState->WX);

            ByteBits IE = mDebugState->IE;
            ImGui::Text("IE: 0x%02X", IE);
            ImGui::SameLine();
            ImGui::Text("IF: 0x%02X", mDebugState->IF);

            ImGui::Text("Interupt Enable Register");
            ImGui::Text("VBlank : %1i", IE.test(VBLANK_BIT));
            ImGui::SameLine();
            ImGui::Text("LCD Status : %1i", IE.test(LCD_STAT_BIT));

            ImGui::Text("Timer : %1i", IE.test(TIMER_BIT));
            ImGui::SameLine();
            ImGui::Text("Serial : %1i", IE.test(SERIAL_BIT));
            ImGui::SameLine();
            ImGui::Text("Joypad : %1i", IE.test(JOYPAD_BIT));
        }

        ImGui::End();
    }

    void Gameboy::UI_Debugger()
    {
        ImGui::Begin("Debug");

        if (ImGui::Button(mDebugState->Paused ? "Play" : "Pause"))
            mDebugState->Paused = !mDebugState->Paused;

        ImGui::SameLine();

        ImGui::Checkbox("Break", &mDebugState->Break);

        ImGui::SameLine();

        ImGui::BeginDisabled(!mDebugState->Paused);
        if (ImGui::Button("Step"))
            UpdateOnce();

        ImGui::EndDisabled();

        static Word sStepSize = 1;
        ImGui::InputScalar("Breakpoint", ImGuiDataType_U16, &mDebugState->Breakpoint, &sStepSize, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);

        ImGui::End();
    }

    void Gameboy::UI_Memory()
    {
        ImGui::Begin("Memory");

        ImGui::BeginChild("MemoryBlock");

        USize elementCount = 0;
        Byte lineBytes[16];

        for (Byte* bytePtr : MemoryMapper::GetMapping())
        {
            Byte byte = bytePtr ? *bytePtr : 0;

            lineBytes[elementCount % 16] = byte;
            elementCount++;

            if (elementCount % 16 == 0)
            {
                std::string line = std::format("0x{:X}\t{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}", (elementCount / 16) * 0x10,
                    lineBytes[0], lineBytes[1], lineBytes[2], lineBytes[3], lineBytes[4], lineBytes[5], lineBytes[6], lineBytes[7],
                    lineBytes[8], lineBytes[9], lineBytes[10], lineBytes[11], lineBytes[12], lineBytes[13], lineBytes[14], lineBytes[15]);

                ImGui::Text(line.c_str());
            }
        }

        ImGui::EndChild();

        ImGui::End();
    }
}
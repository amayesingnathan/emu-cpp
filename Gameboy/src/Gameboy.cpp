module;

#include <imgui.h>

module Gameboy;

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
    }

    Gameboy::~Gameboy()
    {
        AddressBus::Shutdown();
    }

    void Gameboy::update()
    {
        mGraphics->startFrame();

        USize cyclesThisUpdate = 0;
        while (cyclesThisUpdate < CYCLES_PER_FRAME)
        {
            Byte cycles = mProcessor->tick();
            cyclesThisUpdate += cycles;
            mGraphics->tick(cycles);
            mProcessor->handleInterrupts();
        }

        mGraphics->endFrame();
    }

    void Gameboy::uiRender()
    {
        UI_CPU();
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
            Byte* ioRegisters = MemoryMapper::GetBlock(MemoryMapper::IO);
            ByteBits IE = ioRegisters[0x80];

            ImGui::Text("IO Registers");

            ImGui::Text("JOYP: 0x%02X", ioRegisters[0x0]);

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
            ImGui::Text("STAT: 0x%02X", ioRegisters[0x41]);

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

    void Gameboy::UI_Memory()
    {
        ImGui::Begin("Memory");



        ImGui::End();
    }
}
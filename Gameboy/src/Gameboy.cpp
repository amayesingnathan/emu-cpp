#include "gbpch.h"
#include "Gameboy.h"

#include "Memory/MemoryManager.h"

namespace GB {

	Gameboy::Gameboy(std::string_view path)
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
        mGraphics.startFrame();

        int cyclesThisUpdate = 0;
        while (cyclesThisUpdate < _CyclesPerFrame)
        {
            Byte cycles = mProcessor.exec();
            cyclesThisUpdate += cycles;
            mProcessor.updateTimers(cycles);
            mGraphics.update(cycles);
            mProcessor.handleInterupts();
        }

        mGraphics.endFrame();
    }
}
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
        int cyclesThisUpdate = 0;

        while (cyclesThisUpdate < _MaxCycles)
        {
            int cycles = mProcessor.exec();
            cyclesThisUpdate += cycles;
            // UpdateTimers(cycles);
            mGraphics.update(cycles);
            // DoInterupts();
        }
        //RenderScreen();
    }
}
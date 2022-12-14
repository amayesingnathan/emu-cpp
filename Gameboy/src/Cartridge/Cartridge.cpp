#include "gbpch.h"
#include "Cartridge.h"

#include "Memory/MemoryManager.h"

namespace GB {

	Cartridge::Cartridge(std::string_view filename)
        : mROM(MemoryManager::GetBlock(MemoryManager::ROM)),
          mRAM(MemoryManager::GetBlock(MemoryManager::CART_RAM))
    {
        std::ifstream fileROM(filename.data(), std::ios::binary);
        GB_ASSERT(fileROM.is_open(), "Could not locate game file!");

        fileROM.seekg(0, std::ios::end);
        USize cartridgeSize = fileROM.tellg();
        GB_ASSERT(cartridgeSize <= ROM_MAX_SIZE, "Gameboy cartridge is too large!");

        fileROM.seekg(0);
        fileROM.read((char*)mROM, cartridgeSize);

        switch (mROM[0x147])
        {
        case 1:
        case 2:
        case 3:
            mMBC = new MBC1;
            break;

        case 5:
        case 6:
            mMBC = new MBC2;
            break;

        default: break;
        };
    }

    Cartridge::~Cartridge()
    {
        delete mMBC;
    }

    Byte& Cartridge::read(Word address)
    {
        if (!mMBC)
            return mROM[address];

        Word mappedAddress = mMBC->mapRead(address);
        if ((address >= 0xA000 && address < 0xC000))
            return mRAM[mappedAddress];

        return mROM[mappedAddress];
    }

    void Cartridge::write(Word address, Byte data)
    {
        if (!mMBC)
            return;

        Word mappedAddress = mMBC->mapWrite(address, data);
        if (mappedAddress == 0)
            return;

        mRAM[mappedAddress] = data;
    }
}
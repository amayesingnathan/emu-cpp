module Gameboy.Cartridge;

import Gameboy.Memory.AddressBus;

namespace GB {

    Cartridge::Cartridge(std::string_view filename)
    {
        std::ifstream fileROM(filename.data(), std::ios::binary);
        Emu::Assert(fileROM.is_open(), "Could not locate game file!");

        fileROM.seekg(0, std::ios::end);
        USize cartridgeSize = fileROM.tellg();
        Emu::Assert(cartridgeSize <= ROM_MAX_SIZE, "Gameboy cartridge is too large!");

        Byte* ROM = AddressBus::ReadROM();

        fileROM.seekg(0);
        fileROM.read((char*)ROM, cartridgeSize);

        switch (ROM[0x147])
        {
        case 1:
        case 2:
        case 3:
            mMemoryBankController = Emu::MakeInstance<MBC1>();
            break;

        case 5:
        case 6:
            mMemoryBankController = Emu::MakeInstance<MBC2>();
            break;

        default: break;
        };
    }

    void Cartridge::write(Word address, Byte val)
    {
        if (!mMemoryBankController)
            return;

        mMemoryBankController->mapWrite(address, val);
    }
}
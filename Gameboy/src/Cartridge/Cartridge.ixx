export module Gameboy.Cartridge;

export import <fstream>;

import Gameboy.Core.Types;
import Gameboy.Cartridge.MBC;

export namespace GB {

    constexpr USize ROM_MAX_SIZE = 0x200000;
    constexpr USize RAM_MAX_SIZE = 0x20000;

    class Cartridge
    {
    public:
        Cartridge(std::string_view filename);

        void write(Word address, Byte val);

    private:
        Emu::Impl<MBC> mMemoryBankController;
    };
}
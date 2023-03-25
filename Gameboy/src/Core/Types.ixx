export module Gameboy.Core.Types;

import <cstdint>;
import <bitset>;
import <memory>;

export import Gameboy.Core.Common;

export namespace GB {

    using Byte  = uint8_t;
    using SByte = int8_t;

    using Word  = uint16_t;
    using SWord  = int16_t;

    using USize = size_t;

	using ByteBits = std::bitset<8>;

	constexpr auto GB_Bit(Byte bit) { return 1 << bit; }
}
export module Gameboy.CPU.OpCode;

export import <functional>;
export import <bitset>;

import Gameboy.Core.Types;

namespace GB {

	export enum class Condition : Byte
	{
		NZ = 0,
		Z = 1,
		NC = 2,
		C = 3,
		Error = 99
	};

	// Used internally, so don't export.
	namespace OpCodeUtils {

		template<Byte Val>
		static consteval bool TestBit(Byte val) { return Val & GB_Bit(val); }
	}

	export template<Byte T>
	struct OpCode
	{
		static constexpr Byte Val = T;

		template<typename... R>
		static consteval bool Test(R... vals)
		{
			if constexpr (sizeof...(R) == 0)
				Emu::SAssert<false>("Need to provide at least one bit argument!");

			return (... && OpCodeUtils::TestBit<T>(vals));
		}

		// Expressing the OpCode as bits, we can use the arrangement to decode the operation.
		// See https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html for more detail.

		// 7 6 5 4 3 2 1 0
		// --- ----- -----				
		//  X	 Y	   Z
		// 
		//	   --- -
		//		P  Q

		static constexpr Byte X = T >> 6;
		static constexpr Byte Y = (T >> 3) & 7;
		static constexpr Byte Z = T & 7;

		static constexpr Byte P = (T >> 4) & 3;
		static constexpr Byte Q = (T >> 3) & 1;

		static constexpr Condition AsCondition(Byte val)
		{
			return (Condition)val;
		}
	};
}
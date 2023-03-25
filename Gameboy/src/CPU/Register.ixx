export module Gameboy.CPU.Register;

import <concepts>;

export import Gameboy.Core.Types;

export namespace GB {

	class ByteRef
	{
	public:
		ByteRef(Word& val, Byte offset) : mValue(val), mOffset(offset) {}

		operator Byte() const { return mValue >> mOffset & 0xFF; }
		Byte value() const { return  mValue >> mOffset & 0xFF; }

		ByteRef& operator=(Byte rhs)
		{
			mValue = (mValue & ~(0x00FF << mOffset)) | (rhs << mOffset);
			return *this;
		}
		ByteRef& operator^=(Byte rhs)
		{
			mValue ^= (rhs << mOffset);
			return *this;
		}
		ByteRef& operator&=(Byte rhs)
		{
			Word tmp = (rhs << mOffset) | ~(0x00FF << mOffset);
			mValue &= tmp;
			return *this;
		}
		ByteRef& operator|=(Byte rhs)
		{
			mValue |= (rhs << mOffset);
			return *this;
		}

	private:
		Word& mValue;
		Byte mOffset;
	};

	class WordRef
	{
	public:
		WordRef(Word& val) : mValue(val) {}

		operator Word() const { return mValue; }
		Word value() const { return mValue; }

		WordRef& operator=(const WordRef& other) 
		{ 
			mValue = other.mValue; 
			return *this;
		}
		WordRef& operator=(Word rhs)
		{
			mValue = rhs;
			return *this;
		}

		WordRef& operator++() { mValue++; return *this; }
		Word operator++(int) 
		{ 
			Word val = mValue;
			mValue++; 
			return val;
		}

		WordRef& operator--() { mValue--; return *this; }
		Word operator--(int)
		{
			Word val = mValue;
			mValue--;
			return val;
		}

		template<std::integral T> requires (sizeof(T) <= 16)
		WordRef& operator+=(T val) 
		{
			mValue += val; 
			return *this;
		}
		template<std::integral T> requires (sizeof(T) <= 16)
		WordRef& operator-=(T val)
		{
			mValue -= val;
			return *this;
		}

	private:
		Word& mValue;
	};

	class Register
	{
	public:
		Register() = default;
		Register(Word val) : mValue(val) {}

		auto l() { return ByteRef(mValue, 0); }
		auto h() { return ByteRef(mValue, 8); }

		operator WordRef() { return WordRef(mValue); }

	private:
		Word mValue;
	};

	enum class ByteReg
	{
		A, F, B, C, D, E, H, L
	};
	enum class WordReg
	{
		AF, BC, DE, HL, SP, PC
	};

	class Registers
	{
	public:
		Registers()
		{
			mRegisters[5] = 0; // Program Counter
		}

		ByteRef operator[](ByteReg reg)
		{
			switch (reg)
			{
			case ByteReg::A: return mRegisters[0].h();
			case ByteReg::F: return mRegisters[0].l();
			case ByteReg::B: return mRegisters[1].h();
			case ByteReg::C: return mRegisters[1].l();
			case ByteReg::D: return mRegisters[2].h();
			case ByteReg::E: return mRegisters[2].l();
			case ByteReg::H: return mRegisters[3].h();
			case ByteReg::L: return mRegisters[3].l();
			}

			Emu::Assert(false, "Invalid read operation!");
			return mRegisters[0].l();
		}
		WordRef operator[](WordReg reg)
		{
			switch (reg)
			{
			case WordReg::AF: return mRegisters[0];
			case WordReg::BC: return mRegisters[1];
			case WordReg::DE: return mRegisters[2];
			case WordReg::HL: return mRegisters[3];
			case WordReg::SP: return mRegisters[4];
			case WordReg::PC: return mRegisters[5];
			}

			Emu::Assert(false, "Invalid read operation!");
			return mRegisters[0];
		}

	private:
		Register mRegisters[6];
	};

	enum class FlagOp
	{
	};

	class FlagRegister
	{
	public:
		static constexpr Byte CARRY_BIT = 4;
		static constexpr Byte H_CARRY_BIT = 5;
		static constexpr Byte SUBTRACT_BIT = 6;
		static constexpr Byte ZERO_BIT = 7;

	public:
		FlagRegister(ByteRef flagReg) : mRegister(flagReg) {}

		bool carry() const { return isSet<CARRY_BIT>(); }
		bool hcarry() const { return isSet<H_CARRY_BIT>(); }
		bool subtr() const { return isSet<SUBTRACT_BIT>(); }
		bool zero() const { return isSet<ZERO_BIT>(); }

		void setCarry(bool toSet) { return setBit<CARRY_BIT>(toSet); }
		void setHCarry(bool toSet) { return setBit<H_CARRY_BIT>(toSet); }
		void setSubtr(bool toSet) { return setBit<SUBTRACT_BIT>(toSet); }
		void setZero(bool toSet) { return setBit<ZERO_BIT>(toSet); }

	private:
		template<Byte Bit>
		bool isSet() const { return mRegister & GB_Bit(Bit); }

		template<Byte Bit>
		void setBit(bool toSet)
		{
			if (toSet)
				mRegister |= GB_Bit(Bit);
			else
				mRegister &= ~GB_Bit(Bit);
		}

	private:
		ByteRef mRegister;
	};
}

namespace std
{
	template<class CharT>
	struct formatter<GB::WordRef, CharT>
	{
		template <typename FormatParseContext>
		constexpr auto parse(FormatParseContext& pc)
		{
			return formatter<GB::Word, CharT>{}.parse(pc);
		}

		template<typename FormatContext>
		auto format(GB::WordRef p, FormatContext& fc)
		{
			return std::format_to(fc.out(), "{}", p.value());
		}
	};

	template<class CharT>
	struct formatter<GB::ByteRef, CharT>
	{
		template <typename FormatParseContext>
		constexpr auto parse(FormatParseContext& pc)
		{
			return formatter<GB::Byte, CharT>{}.parse(pc);
		}

		template<typename FormatContext>
		auto format(GB::ByteRef p, FormatContext& fc)
		{
			return std::format_to(fc.out(), "{}", p.value());
		}
	};
}
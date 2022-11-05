#pragma once

#include "Core/Types.h"

namespace GB {

	class Memory
	{
	public:
		enum Type
		{
			WRAM, VRAM, ROM, OAM, IO, BLOCK_COUNT
		};

		GB_CONST Flag _Read = 0;
		GB_CONST Flag _Write = 1;

	public:
		Memory(Type type);

	protected:
		virtual constexpr USize GetSize() const = 0;
		virtual Byte& GetMemBlock(Word address) = 0;

		Flag GetMode() const { return mMode; }

	private:
		Byte read(Word address) { mMode = _Read; return GetMemBlock(address); }
		void write(Word address, Byte data) { mMode = _Write; GetMemBlock(address) = data; }

	private:
		Flag mMode = _Read;

		friend class AddressBus;
	};

}
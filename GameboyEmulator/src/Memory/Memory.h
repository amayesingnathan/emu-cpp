#pragma once

#include "Core/Types.h"

namespace GB {

	class Memory
	{
	protected:
		enum Type
		{
			BOOTSTRAP, WRAM, VRAM, ROM, OAM, IO, BLOCK_COUNT
		};

		enum Mode
		{
			Read = 0,
			Write = 1
		};

	public:
		Memory(Type type);

	protected:
		virtual constexpr USize GetSize() const = 0;
		virtual Byte& GetMemBlock(Word address) = 0;

		Mode GetMode() const { return mMode; }

	private:
		Byte read(Word address) { mMode = Read; return GetMemBlock(address); }
		void write(Word address, Byte data) { mMode = Write; GetMemBlock(address) = data; }

	private:
		Mode mMode = Read;

		friend class AddressBus;
	};

}
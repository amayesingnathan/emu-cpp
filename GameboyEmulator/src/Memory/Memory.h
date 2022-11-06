#pragma once

#include "Address.h"

namespace GB {

	class Memory
	{
	public:
		enum Mode
		{
			Read = 0,
			Write = 1
		};

	protected:
		enum Type
		{
			BOOTSTRAP, WRAM, VRAM, ROM, OAM, IO, BLOCK_COUNT
		};

	public:
		Memory(Type type);

	protected:
		virtual constexpr USize GetSize() const = 0;
		virtual Byte& GetMemBlock(Address address) = 0;

		Mode GetMode() const { return mMode; }

	private:
		Byte read(Address address) { mMode = Read; return GetMemBlock(address); }
		void write(Address address, Byte data) { mMode = Write; GetMemBlock(address) = data; }

	private:
		Mode mMode = Read;

		friend class AddressBus;
	};

}
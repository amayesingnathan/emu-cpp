#pragma once

#include "Core/Types.h"

namespace GB {

	class Memory
	{
	public:
		enum Type
		{
			WRAM, VRAM, ROM, LAST
		};

	public:
		Memory(Type type);

	protected:
		virtual Byte& GetMemBlock(Word address) = 0;

	private:
		Byte read(Word address) { return GetMemBlock(address); }
		void write(Word address, Byte data) { GetMemBlock(address) = data; }

		friend class AddressBus;
	};

}
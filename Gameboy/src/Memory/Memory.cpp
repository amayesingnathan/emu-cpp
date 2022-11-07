#include "gbpch.h"
#include "Memory.h"

#include "AddressBus.h"

namespace GB {

	Memory::Memory(Type type)
	{
		AddressBus::Register(type, this);
	}
}
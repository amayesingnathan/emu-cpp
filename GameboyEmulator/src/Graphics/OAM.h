#pragma once

#include "Memory/Memory.h"

namespace GB {

	class OAM : public Memory
	{
	public:
		static constexpr USize _OAMSize = 0xA0;

	protected:
		Byte* GetMemBlock() override { return mData; }

	private:
		Byte mData[_OAMSize];
	};
}
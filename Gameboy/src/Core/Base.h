#pragma once

#include "gbpch.h"
#include "EmuBase.h"

#define GB_ASSERT(x, ...) assert(x)
#define GB_SASSERT(...) static_assert(__VA_ARGS__)

#define GB_CONST static constexpr

#define if_c if constexpr

#define GB_BIG_ENDIAN std::endian::native == std::endian::big
#define GB_LITTLE_ENDIAN std::endian::native == std::endian::little

#define GB_BIT(x) (1 << x)

#define GB_BIND_DISPATCH(func, instr) std::bind(&CPU::func, this, instr)
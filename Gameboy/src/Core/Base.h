#pragma once

#include "gbpch.h"
#include "EmuBase.h"

#define GB_ASSERT(x, ...) assert(x)
#define GB_SASSERT(x, ...) static_assert(x)

#define GB_CONST static constexpr

#define if_c if constexpr

#define GB_BIG_ENDIAN std::endian::native == std::endian::big
#define GB_LITTLE_ENDIAN std::endian::native == std::endian::little

#define GB_BIT(x) (1 << x)

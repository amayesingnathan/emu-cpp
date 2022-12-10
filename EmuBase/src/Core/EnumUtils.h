#pragma once

#include "magic_enum.hpp"

#include "Core/Common.h"

namespace Emu::Enum {

	template<typename T>
	inline static constexpr std::string_view ToString(T _enum)
	{
		GL_SASSERT(MAGIC_ENUM_SUPPORTED, "Compiler does not support magic enums! Define your own conversions!");
		GL_SASSERT(std::is_enum<T>::value, "Argument must be an enum!");

		return magic_enum::enum_name(_enum);
	}

	template<typename T>
	inline static constexpr T FromString(std::string_view _enumStr)
	{
		GL_SASSERT(MAGIC_ENUM_SUPPORTED, "Compiler does not support magic enums! Define your own conversions!");

		auto _enum = magic_enum::enum_cast<T>(_enumStr);
		if (_enum.has_value())
			return _enum.value();

		return T::None;
	}
}
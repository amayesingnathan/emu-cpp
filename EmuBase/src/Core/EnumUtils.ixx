module;

#include "magic_enum.hpp"

export module EmuBase.Core.Enum;

import EmuBase.Core.Common;

export namespace Emu::Enum {

	template<typename T>
	concept MagicEnumEnabled = (bool)MAGIC_ENUM_SUPPORTED && std::is_enum_v<T>;

	template<typename T> requires MagicEnumEnabled<T>
	constexpr std::string_view ToString(T _enum)
	{
		SAssert<MAGIC_ENUM_SUPPORTED>("Compiler does not support magic enums! Define your own conversions!");
		SAssert<std::is_enum<T>::value>("Argument must be an enum!");

		return magic_enum::enum_name(_enum);
	}

	template<typename T> requires MagicEnumEnabled<T>
	constexpr T FromString(std::string_view _enumStr)
	{
		SAssert<MAGIC_ENUM_SUPPORTED>("Compiler does not support magic enums! Define your own conversions!");

		auto _enum = magic_enum::enum_cast<T>(_enumStr);
		if (_enum.has_value())
			return _enum.value();

		return T::None;
	}
}
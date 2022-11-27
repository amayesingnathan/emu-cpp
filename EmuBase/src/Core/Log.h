#pragma once

#include "Common.h"

#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Emu {

	class LogWidget;

	class Log
	{
	public:
		static void Init(LogWidget& widget);

		static Ref<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
		static Ref<spdlog::logger>& GetEmuLogger() { return sEmuLogger; }

	private:
		inline static Ref<spdlog::logger> sCoreLogger = nullptr;
		inline static Ref<spdlog::logger> sEmuLogger = nullptr;
	};
}

#define NO_RELEASE_LOGGING 0

#if defined(EMU_RELEASE) && NO_RELEASE_LOGGING
	#define EMU_CORE_TRACE(...)
	#define EMU_CORE_INFO(...)
	#define EMU_CORE_WARN(...)
	#define EMU_CORE_ERROR(...)

	#define EMU_TRACE(...)
	#define EMU_INFO(...)
	#define EMU_WARN(...)
	#define EMU_ERROR(...)
#else
	#define EMU_CORE_TRACE(...)		::Emu::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define EMU_CORE_INFO(...)		::Emu::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define EMU_CORE_WARN(...)		::Emu::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define EMU_CORE_ERROR(...)		::Emu::Log::GetCoreLogger()->error(__VA_ARGS__)

	#define EMU_TRACE(...)			::Emu::Log::GetEmuLogger()->trace(__VA_ARGS__)
	#define EMU_INFO(...)			::Emu::Log::GetEmuLogger()->info(__VA_ARGS__)
	#define EMU_WARN(...)			::Emu::Log::GetEmuLogger()->warn(__VA_ARGS__)
	#define EMU_ERROR(...)			::Emu::Log::GetEmuLogger()->error(__VA_ARGS__)
#endif
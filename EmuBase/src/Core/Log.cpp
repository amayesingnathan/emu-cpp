#include "glpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "ImGui/ImGuiLogger.h"

namespace Emu {

	void Log::Init(LogWidget& widget)
	{
		std::vector<spdlog::sink_ptr> logSinks;
		//logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("EmuCpp.log", true));
		logSinks.emplace_back(std::make_shared<ImGuiSink>(widget));

		//logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[0]->set_pattern("[%T] [%l] %n: %v");
		logSinks[1]->set_pattern("%^[%T] %n: %v%$");

		sCoreLogger = std::make_shared<spdlog::logger>("EmuCpp", begin(logSinks), end(logSinks));
		spdlog::register_logger(sCoreLogger);
		sCoreLogger->set_level(spdlog::level::trace);
		sCoreLogger->flush_on(spdlog::level::trace);

		sEmuLogger = std::make_shared<spdlog::logger>("Emulator", begin(logSinks), end(logSinks));
		spdlog::register_logger(sEmuLogger);
		sEmuLogger->set_level(spdlog::level::trace);
		sEmuLogger->flush_on(spdlog::level::trace);
	}
}
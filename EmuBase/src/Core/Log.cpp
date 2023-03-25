module;

#pragma warning (push)
#pragma warning(disable: 4005)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

module EmuBase.Core.Log;

import EmuBase.ImGui.Sink;

namespace Emu {

	class Logger
	{
	public:
		static void Init(LogWidget& widget);

		static Ref<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
		static Ref<spdlog::logger>& GetEmuLogger() { return sEmuLogger; }

	private:
		inline static Ref<spdlog::logger> sCoreLogger = nullptr;
		inline static Ref<spdlog::logger> sEmuLogger = nullptr;
	};

	void Log::Init(LogWidget& widget)
	{
		Logger::Init(widget);
	}

	void Log::Trace(std::string_view msg)
	{
		Logger::GetCoreLogger()->trace(msg);
	}

	void Log::Info(std::string_view msg)
	{
		Logger::GetCoreLogger()->info(msg);
	}

	void Log::Warn(std::string_view msg)
	{
		Logger::GetCoreLogger()->warn(msg);
	}

	void Log::Error(std::string_view msg)
	{
		Logger::GetCoreLogger()->error(msg);
	}

	void Logger::Init(LogWidget& widget)
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
#pragma warning(pop)
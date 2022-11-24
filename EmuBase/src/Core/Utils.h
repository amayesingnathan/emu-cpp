#pragma once

#include "Common.h"

namespace Emu {

	using Timepoint = std::chrono::steady_clock::time_point;
	using Duration = std::chrono::duration<float, std::milli>;

	class Time
	{
	public:
		inline static void Begin() { sTimepoint = Now(); }

		inline static Duration Elapsed() { return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(Now() - sTimepoint); }

		inline static void Sleep(Duration milliseconds) { std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(milliseconds)); }

	private:
		inline static Timepoint Now() { return std::chrono::steady_clock::now(); }

	private:
		inline static Timepoint sTimepoint = Now();
	};

}
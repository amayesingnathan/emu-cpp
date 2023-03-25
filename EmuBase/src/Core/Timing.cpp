module EmuBase.Core.Timing;

namespace Emu {

	void Time::Begin()
	{
		sTimepoint = Now();
	}

	Duration Time::Elapsed()
	{
		return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(Now() - sTimepoint);
	}

	void Time::Sleep(Duration milliseconds)
	{
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(milliseconds));
	}

	inline Timepoint Time::Now()
	{
		return std::chrono::steady_clock::now();
	}
}

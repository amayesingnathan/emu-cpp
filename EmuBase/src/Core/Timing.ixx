export module EmuBase.Core.Timing;

export import <chrono>;

export namespace Emu {

	using Timepoint = std::chrono::steady_clock::time_point;
	using Duration = std::chrono::duration<float, std::milli>;

	class Time
	{
	public:
		static void Begin();
		static Duration Elapsed();
		static void Sleep(Duration milliseconds);

	private:
		static Timepoint Now();

	private:
		inline static Timepoint sTimepoint;
	};

}
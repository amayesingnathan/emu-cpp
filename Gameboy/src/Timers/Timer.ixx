export module Gameboy.Timers;

import Gameboy.CPU.Register;

export namespace GB {

	constexpr USize	CLOCK_SPEED = 4194304;

	constexpr Word FREQUENCIES[4] =
	{
		CLOCK_SPEED / 4096,
		CLOCK_SPEED / 262144,
		CLOCK_SPEED / 65536,
		CLOCK_SPEED / 16382
	};

	constexpr Byte FREQ_4096   = 0x00;
	constexpr Byte FREQ_262144 = 0x01;
	constexpr Byte FREQ_65536  = 0x02;
	constexpr Byte FREQ_16384  = 0x03;

	class Timer
	{
	public:
		Timer(Byte& reg, Byte freq)
			: mRegister(reg), mFrequency(freq) 
		{
			mRegister = 0;
		}

		bool step(Byte cycles)
		{
			if (!mRunning)
				return false;

			const Word frequency = FREQUENCIES[mFrequency];

			mCounter += cycles;
			if (mCounter < frequency)
				return false;

			mCounter -= frequency;
			mRegister++;

			if (mRegister == 0x00) //Overflow
				return true;

			return false;
		}

		void start() { mRunning = true; }
		void stop() { mRunning = false; }

		void setFreq(Byte freq) { mFrequency = freq; }

		Timer& operator=(Byte val)
		{
			mRegister = val;
			return *this;
		}

	private:
		bool mRunning = true;

		Byte& mRegister;
		Word mCounter = 0;

		Byte mFrequency;
	};
}
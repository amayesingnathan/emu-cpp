#pragma once

namespace GB {

	class MBC
	{
	public:
		enum Mode
		{
			READ, WRITE
		};

	public:
		virtual ~MBC() {}

		virtual Word map(Mode mode, Word address) = 0;
	};

	class MBC1 : public MBC
	{
		Word map(Mode mode, Word address) override { return 0; }
	};

	class MBC2 : public MBC
	{
		Word map(Mode mode, Word address) override { return 0; }
	};
}
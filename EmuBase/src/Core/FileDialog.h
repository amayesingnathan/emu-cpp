#pragma once

#include "Core/Common.h"

namespace Emu {

	namespace fs = std::filesystem;

	namespace File
	{
		fs::path OpenFile(const std::vector<std::string>& filter);
		fs::path OpenDir();
		fs::path SaveFile(const std::vector<std::string>& filter);
	};
}
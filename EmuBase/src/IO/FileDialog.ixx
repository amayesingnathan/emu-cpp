export module EmuBase.IO.File;

import EmuBase.Core.Common;
export import <filesystem>;

export namespace Emu {

	namespace fs = std::filesystem;

	class File
	{
	public:
		static fs::path OpenFile(const std::vector<std::string>& filter);
		static fs::path OpenDir();
		static fs::path SaveFile(const std::vector<std::string>& filter);
	};
}
module;

#pragma warning (push)
#pragma warning(disable: 4005)
#include <portable-file-dialogs.h>

module EmuBase.IO.File;

namespace Emu {

	fs::path File::OpenFile(const std::vector<std::string>& filter)
	{
		auto selection = pfd::open_file("Select a file", ".", filter).result();
		if (!selection.empty())
			return selection[0];

		return fs::path();
	}

	fs::path File::OpenDir()
	{
		return pfd::select_folder("Select a folder", ".").result();
	}

	fs::path File::SaveFile(const std::vector<std::string>& filter)
	{
		return pfd::save_file("Save file as", ".", filter).result();
	}
}
#pragma warning (pop)
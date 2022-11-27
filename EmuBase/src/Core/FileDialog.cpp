#include "glpch.h"
#include "FileDialog.h"

#include <portable-file-dialogs.h>

namespace Emu::File {

	fs::path OpenFile(const std::vector<std::string>& filter)
	{
		auto selection = pfd::open_file("Select a file", ".", filter).result();
		if (!selection.empty())
			return selection[0];

		return fs::path();
	}
	fs::path OpenDir()
	{
		return pfd::select_folder("Select a folder", ".").result();
	}
	fs::path SaveFile(const std::vector<std::string>& filter)
	{
		return pfd::save_file("Save file as", ".", filter).result();
	}
}
#include <string>
#include <Windows.h>
#include "../STL/replace_all.cpp"

[[nodiscard]] bool is_ghost_path(std::wstring path) {
	// "/" -> "\\"
	replace_all(path, L"/", L"\\");
	// "\\\\" -> "\\"
	replace_all(path, L"\\\\", L"\\");
	if(path.ends_with(L"ghost\\master\\")) {
		auto descript_txt_path = path + L"descript.txt";
		return _waccess(descript_txt_path.c_str(), 0) == 0;
	}
	return false;
}

[[nodiscard]] bool path_in_ghost_dir(std::wstring path) {
	// "/" -> "\\"
	replace_all(path, L"/", L"\\");
	// "\\\\" -> "\\"
	replace_all(path, L"\\\\", L"\\");
	if(path.contains(L"ghost\\master\\")) {
		auto ghost_path		   = path.substr(0, path.find(L"ghost\\master\\") + 13);
		auto descript_txt_path = ghost_path + L"descript.txt";
		return _waccess(descript_txt_path.c_str(), 0) == 0;
	}
	return false;
}

[[nodiscard]] std::wstring get_ghost_path(std::wstring path) {
	// "/" -> "\\"
	replace_all(path, L"/", L"\\");
	// "\\\\" -> "\\"
	replace_all(path, L"\\\\", L"\\");
	if(path.contains(L"ghost\\master\\")) {
		auto ghost_path		   = path.substr(0, path.find(L"ghost\\master\\") + 13);
		auto descript_txt_path = ghost_path + L"descript.txt";
		if(_waccess(descript_txt_path.c_str(), 0) == 0) {
			return ghost_path;
		}
	}
	return {};
}

[[nodiscard]] std::wstring make_ghost_path(std::wstring path) {
	// "/" -> "\\"
	replace_all(path, L"/", L"\\");
	// "\\\\" -> "\\"
	replace_all(path, L"\\\\", L"\\");
	if(!path.ends_with(L"ghost\\master\\"))
		path += L"\\ghost\\master\\";
	{
		auto descript_txt_path = path + L"descript.txt";
		if(_waccess(descript_txt_path.c_str(), 0) == 0) {
			return path;
		}
	}
	return {};
}

[[nodiscard]] std::wstring get_ghost_dir_name(std::wstring path) {
	// "/" -> "\\"
	replace_all(path, L"/", L"\\");
	// "\\\\" -> "\\"
	replace_all(path, L"\\\\", L"\\");
	if(path.contains(L"ghost\\master\\")) {
		auto ghost_path		   = path.substr(0, path.find(L"ghost\\master\\") + 13);
		auto descript_txt_path = ghost_path + L"descript.txt";
		if(_waccess(descript_txt_path.c_str(), 0) == 0) {
			//remove ghost\\master\\ from path
			auto ghost_dir_name = ghost_path.substr(0, ghost_path.find(L"\\ghost\\master\\"));
			ghost_dir_name=ghost_dir_name.substr(ghost_dir_name.find_last_of(L"\\")+1);
			return ghost_dir_name;
		}
	}
	return {};
}

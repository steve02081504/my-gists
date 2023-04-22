#pragma once
#include <string>
#include <windows.h>

namespace from_ghost_path{
	using namespace std;
	struct name_and_icon_path_t {
		wstring name;
		wstring icon_path;
	};
	name_and_icon_path_t get_name_and_icon_path(wstring ghost_path);
	wstring get_name(wstring ghost_path);
	wstring get_shiori_path(wstring ghost_path);
	HICON load_icon(wstring ghost_path, wstring icon_path);
}

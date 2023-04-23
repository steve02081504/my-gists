#pragma once
#include <string>

namespace from_ghost_path{
	using namespace std;
	struct name_and_icon_path_t {
		wstring name;
		wstring icon_path;
	};
	name_and_icon_path_t get_name_and_icon_path(wstring ghost_path);
	wstring get_name(wstring ghost_path);
	wstring get_shiori_path(wstring ghost_path);
}

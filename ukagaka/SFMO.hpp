#pragma once
#include <string>
#include <map>
struct SFMO_obj_t{
	std::wstring ID;
	std::map<std::wstring, std::wstring> map;
	auto& operator[](std::wstring a) { return map[a]; }

	[[nodiscard]] std::wstring get_modulestate(std::wstring a);
};
struct SFMO_t{
	std::map<std::wstring,SFMO_obj_t> info_map;
	
	bool Update_info();
	[[nodiscard]] HWND wait_ghost_info_by_name(std::wstring ghost_name, size_t wait_time);
	[[nodiscard]] HWND wait_ghost_info_by_fullname(std::wstring ghost_name, size_t wait_time);
	[[nodiscard]] HWND wait_ghost_info_by_name_or_fullname(std::wstring ghost_name, size_t wait_time);
};

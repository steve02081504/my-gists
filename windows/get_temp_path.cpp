#pragma once
#include <fileapi.h>
#include <string>
const wchar_t* get_temp_path() {
	static wchar_t temp_path[MAX_PATH + 1]{};
	static bool	   temp_path_initer = (bool)GetTempPath(MAX_PATH, temp_path);
	return temp_path;
}
const wchar_t* temp_filename_gen_header();
std::wstring   get_temp_filename(const wchar_t* file_suffix = L"") {
	  //generate a temporary file name
	  std::wstring temp_file_name;
	  temp_file_name.resize(MAX_PATH);
	  GetTempFileNameW(get_temp_path(), temp_filename_gen_header(), 0, temp_file_name.data());
	  temp_file_name.resize(wcslen(temp_file_name.data()));
	  if(!*file_suffix) {
		  //append file suffix
		  temp_file_name += file_suffix;
	  }
	  return temp_file_name;
}
std::wstring get_temp_filename(const std::wstring& file_suffix) {
	return get_temp_filename(file_suffix.c_str());
}

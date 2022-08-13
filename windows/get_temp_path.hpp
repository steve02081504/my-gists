#pragma once
#include <string>
const wchar_t* get_temp_path();
const wchar_t* temp_filename_gen_header();
std::wstring get_temp_filename(const std::wstring& file_suffix={});

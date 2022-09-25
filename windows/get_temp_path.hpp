#pragma once
#include <string>
const wchar_t* get_temp_path();
const wchar_t* temp_filename_gen_header();
std::wstring   get_temp_filename(const wchar_t* file_suffix = L"");
std::wstring   get_temp_filename(const std::wstring& file_suffix);

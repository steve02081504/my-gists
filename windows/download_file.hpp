#pragma once
#include <string>
const wchar_t* NetAgentName();
std::wstring download_file(const std::wstring& url, const std::wstring& file);
std::wstring download_temp_file(const std::wstring& url, const std::wstring& file_suffix);
std::wstring download_file_to_temp_dir(const std::wstring& url, const std::wstring& filename);

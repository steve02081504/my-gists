#pragma once
#include <string>
const wchar_t* NetAgentName();
std::wstring   download_file(const wchar_t* url, const wchar_t* file);
std::wstring   download_file(const std::wstring& url, const wchar_t* file);
std::wstring   download_file(const wchar_t* url, const std::wstring& file);
std::wstring   download_file(const std::wstring& url, const std::wstring& file);

std::wstring download_temp_file(const std::wstring& url, const std::wstring& file_suffix);
std::wstring download_temp_file(const wchar_t* url, const std::wstring& file_suffix);
std::wstring download_temp_file(const std::wstring& url, const wchar_t* file_suffix);
std::wstring download_temp_file(const wchar_t* url, const wchar_t* file_suffix);
std::wstring download_temp_file(const std::wstring& url);
std::wstring download_temp_file(const wchar_t* url);

std::wstring download_file_to_temp_dir(const std::wstring& url, const std::wstring& filename);
std::wstring download_file_to_temp_dir(const wchar_t* url, const std::wstring& filename);
std::wstring download_file_to_temp_dir(const std::wstring& url, const wchar_t* filename);
std::wstring download_file_to_temp_dir(const wchar_t* url, const wchar_t* filename);

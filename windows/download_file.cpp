#pragma once
#include <string>
#include <stdexcept>
#include <windows.h>
#include <wininet.h>
#include "get_temp_path.cpp"
#include "download_file.hpp"
#include "../codepage.hpp"
#pragma comment(lib, "WinInet.lib")
std::wstring get_domain_form_url(const std::wstring& url)
{
	std::wstring domain;
	auto		 domain_begin = url.find(L"://");
	if (domain_begin != std::wstring::npos)
		domain_begin += 3;
	else
		domain_begin = url.find(L':');
	if (domain_begin != std::wstring::npos) {
		auto domain_end = url.find_first_of(L"/\\", domain_begin);
		if (domain_end == std::wstring::npos)
			domain_end = url.length();
		domain = url.substr(domain_begin, domain_end - domain_begin);
	}
	return domain;
}
std::wstring download_file(const std::wstring& url, const std::wstring& file) {
	if(_waccess(file.c_str(), 0) == 0) {
		return file;
	}
	//
	HANDLE hFileHandle = CreateFileW(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if(hFileHandle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("CreateFileW failed");
	}
	//download file
	HINTERNET hInternet = InternetOpenW(NetAgentName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hInternet == NULL) {
		throw std::runtime_error("InternetOpenW failed");
	}
	HINTERNET hFile = InternetOpenUrlW(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if(hFile == NULL) {
		InternetCloseHandle(hInternet);
		auto domain = get_domain_form_url(url);
		if(domain.empty())
			throw std::runtime_error("InternetOpenUrlW failed");
		else
			throw std::runtime_error("InternetOpenUrlW failed, Please ensure that you can link to " + CODEPAGE_n::UnicodeToMultiByte(domain,0));
	}
	DWORD		  dwBytesRead	 = 0;
	DWORD		  dwBytesWritten = 0;
	unsigned char buffer[1024];
	bool		  full_download = false;
	while(InternetReadFile(hFile, buffer, 1024, &dwBytesRead)) {
		if(dwBytesRead > 0)
			WriteFile(hFileHandle, buffer, dwBytesRead, &dwBytesWritten, NULL);
		else {
			full_download = true;
			break;
		}
	}
	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);
	CloseHandle(hFileHandle);
	if(!full_download) {
		DeleteFileW(file.c_str());
		throw std::runtime_error("InternetReadFile failed");
	}
	return file;
}
std::wstring download_temp_file(const std::wstring& url, const std::wstring& file_suffix) {
	return download_file(url, get_temp_filename(file_suffix));
}
std::wstring download_file_to_temp_dir(const std::wstring& url, const std::wstring& filename) {
	auto filepath = get_temp_path() + filename;
	return download_file(url, filepath);
}

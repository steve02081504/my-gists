#pragma once
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <fstream>
#include "CutSpace.cpp"
#if defined(_WIN32)
	#include "../codepage.hpp"
	#include <wininet.h>
	#pragma comment(lib, "WinInet.lib")
#endif
#include "yaml_reader.hpp"

bool yaml_reader::read_line(std::wstring line) {
	CutSpace(line);
	if(line[0] == L'-') {
		reading_index++;
		_data.resize(reading_index + 1);
		//remove '-'
		line = line.substr(1);
		CutSpace(line);
	}
	{
		size_t pos = line.find(L':');
		if(pos == std::wstring::npos) {
			return false;
		}
		std::wstring key		  = line.substr(0, pos);
		std::wstring value		  = line.substr(pos + 1);
		_data[reading_index][key] = CutSpace(value);
	}
}

void yaml_reader::reader_init() {
	_data.clear();
	reading_index = size_t(-1);
}

void yaml_reader::read_file(const std::wstring& file_path) {
	std::wifstream file(file_path);
	if(!file.is_open()) {
		throw std::runtime_error("file not found");
	}
	reader_init();
	std::wstring line;
	while(std::getline(file, line)) {
		read_line(line);
	}
}

#if defined(_WIN32)
bool yaml_reader::read_url(const std::wstring& url) {
	reader_init();
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
			throw std::runtime_error("InternetOpenUrlW failed, Please ensure that you can link to " + CODEPAGE_n::UnicodeToMultiByte(domain, 0));
	}
	char				 buffer[1024];
	std::wstring		 line;
	DWORD				 dwBytesRead   = 0;
	bool				 full_download = false;
	CODEPAGE_n::CODEPAGE code_page	   = CODEPAGE_n::CP_UTF8;
	while(InternetReadFile(hFile, buffer, 1024, &dwBytesRead)) {
		if(dwBytesRead > 0) {
			line += CODEPAGE_n::MultiByteToUnicode(buffer, code_page);
			auto line_end = line.find(L'\n');
			while(line_end != std::wstring::npos) {
				read_line(line.substr(0, line_end));
				line	 = line.substr(line_end + 1);
				line_end = line.find(L'\n');
			}
		}
		else {
			full_download = true;
			break;
		}
	}
	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);
	return full_download;
}
#endif

yaml_reader::yaml_map yaml_reader::find(std::wstring key, std::wstring value) {
	for(auto& map: _data) {
		auto it = map.find(key);
		if(it != map.end() && it->second == value) {
			return map;
		}
	}
	return yaml_map();
}

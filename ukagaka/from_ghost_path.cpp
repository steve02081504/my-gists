#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <Windows.h>

#include "../codepage.cpp"
#include "../STL/Split.cpp"

#include <shlwapi.h>//PathFileExistsW
//lib of PathFileExists
#pragma comment(lib, "shlwapi.lib")

namespace from_ghost_path{
	using namespace std;
	struct name_and_icon_path_t {
		wstring name;
		wstring icon_path;
	};
	name_and_icon_path_t get_name_and_icon_path(wstring ghost_path) {
		auto descript_name = ghost_path + L"\\descript.txt";
		auto descript_f	   = _wfopen(descript_name.c_str(), L"rb");
		name_and_icon_path_t aret;
		//
		CODEPAGE_n::CODEPAGE cp = CODEPAGE_n::CP_UTF8;
		char				 buf[2048];
		wstring				 line, s0, s1;
		if(descript_f) {
			while(fgets(buf, 2048, descript_f)) {
				line	 = CODEPAGE_n::MultiByteToUnicode(buf, cp);
				auto len = line.size();
				if(len && *line.rbegin() == L'\n')
					line.resize(--len);
				if(len && *line.rbegin() == L'\r')
					line.resize(--len);
				Split(line, s0, s1, L",");
				if(s0 == L"charset")
					cp = CODEPAGE_n::StringtoCodePage(s1.c_str());
				else if(s0 == L"icon") {
					aret.icon_path = ghost_path + L'\\' + s1;
					if(!PathFileExistsW(aret.icon_path.c_str()))
						aret.icon_path.clear();
					if(!aret.name.empty()) {
						fclose(descript_f);
						return aret;
					}
				}
				else if(s0 == L"name") {
					aret.name = s1;
					if(!aret.icon_path.empty()) {
						fclose(descript_f);
						return aret;
					}
				}
			}
			fclose(descript_f);
		}
		return aret;
	}
	wstring get_name(wstring ghost_path) {
		auto descript_name = ghost_path + L"\\descript.txt";
		auto descript_f	   = _wfopen(descript_name.c_str(), L"rb");
		//
		CODEPAGE_n::CODEPAGE cp = CODEPAGE_n::CP_UTF8;
		char				 buf[2048];
		wstring				 line, s0, s1;
		if(descript_f) {
			while(fgets(buf, 2048, descript_f)) {
				line	 = CODEPAGE_n::MultiByteToUnicode(buf, cp);
				auto len = line.size();
				if(len && *line.rbegin() == L'\n')
					line.resize(--len);
				if(len && *line.rbegin() == L'\r')
					line.resize(--len);
				Split(line, s0, s1, L",");
				if(s0 == L"charset")
					cp = CODEPAGE_n::StringtoCodePage(s1.c_str());
				else if(s0 == L"name") {
					fclose(descript_f);
					return s1;
				}
			}
			fclose(descript_f);
		}
		return {};
	}
	wstring get_shiori_path(wstring ghost_path) {
		auto descript_name = ghost_path + L"\\descript.txt";
		auto descript_f	   = _wfopen(descript_name.c_str(), L"rb");
		//
		CODEPAGE_n::CODEPAGE cp = CODEPAGE_n::CP_UTF8;
		char				 buf[2048];
		wstring				 line, s0, s1;
		if(descript_f) {
			while(fgets(buf, 2048, descript_f)) {
				line	 = CODEPAGE_n::MultiByteToUnicode(buf, cp);
				auto len = line.size();
				if(len && *line.rbegin() == L'\n')
					line.resize(--len);
				if(len && *line.rbegin() == L'\r')
					line.resize(--len);
				Split(line, s0, s1, L",");
				if(s0 == L"charset")
					cp = CODEPAGE_n::StringtoCodePage(s1.c_str());
				else if(s0 == L"shiori") {
					fclose(descript_f);
					return ghost_path + s1;
				}
			}
			fclose(descript_f);
		}
		return {};
	}
}

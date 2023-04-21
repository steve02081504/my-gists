#pragma once
#include <string>
#include "CutSpace.cpp"
bool Split(std::wstring& str, std::wstring& s0, std::wstring& s1, std::wstring_view sepstr) {
	// strをs0とs1に分解
	auto begin = str.find(sepstr);
	s0		   = str.substr(0, begin);
	s1		   = str.substr(begin + sepstr.size());
	CutSpace(s0);
	CutSpace(s1);

	return begin != std::wstring::npos;
}

bool Split(std::string& str, std::string& s0, std::string& s1, std::string_view sepstr) {
	// strをs0とs1に分解
	auto begin = str.find(sepstr);
	s0		   = str.substr(0, begin);
	s1		   = str.substr(begin + sepstr.size());
	CutSpace(s0);
	CutSpace(s1);

	return begin != std::string::npos;
}

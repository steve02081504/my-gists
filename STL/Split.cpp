#pragma once
#include <string>
#include "CutSpace.cpp"
bool Split(const std::wstring& str, std::wstring& s0, std::wstring& s1, std::wstring_view sepstr) {
	// strをs0とs1に分解
	const auto begin = str.find(sepstr);
	s0		   = str.substr(0, begin);
	s1		   = str.substr(begin + sepstr.size());
	CutSpace(s0);
	CutSpace(s1);

	return begin != std::wstring::npos;
}

bool Split(const std::string& str, std::string& s0, std::string& s1, std::string_view sepstr) {
	// strをs0とs1に分解
	const auto begin = str.find(sepstr);
	s0		   = str.substr(0, begin);
	s1		   = str.substr(begin + sepstr.size());
	CutSpace(s0);
	CutSpace(s1);

	return begin != std::string::npos;
}

//string_view
bool Split(const std::wstring_view& str, std::wstring_view& s0, std::wstring_view& s1, std::wstring_view sepstr) {
	// strをs0とs1に分解
	const auto begin = str.find(sepstr);
	s0		   = str.substr(0, begin);
	s1		   = str.substr(begin + sepstr.size());
	CutSpace(s0);
	CutSpace(s1);

	return begin != std::wstring::npos;
}

bool Split(const std::string_view& str, std::string_view& s0, std::string_view& s1, std::string_view sepstr) {
	// strをs0とs1に分解
	const auto begin = str.find(sepstr);
	s0		   = str.substr(0, begin);
	s1		   = str.substr(begin + sepstr.size());
	CutSpace(s0);
	CutSpace(s1);

	return begin != std::string::npos;
}

//string & string_view
bool Split(const std::wstring& str, std::wstring_view& s0, std::wstring_view& s1, std::wstring_view sepstr){
    return Split(std::wstring_view(str), s0, s1, sepstr);
}
bool Split(const std::string& str, std::string_view& s0, std::string_view& s1, std::string_view sepstr){
    return Split(std::string_view(str), s0, s1, sepstr);
}

#pragma once
#include <string>
std::wstring &CutSpace(std::wstring &str) {
	auto pos1 = str.find_first_not_of(L" \t");
	auto pos2 = str.find_last_not_of(L" \t");
	if(pos1 == std::wstring::npos) {
		str = L"";
		return str;
	}
	if(pos1 == pos2) {
		str = str.substr(pos1);
		return str;
	}
	str = str.substr(pos1, pos2 - pos1 + 1);
	return str;
};

std::string &CutSpace(std::string &str) {
	auto pos1 = str.find_first_not_of(" \t");
	auto pos2 = str.find_last_not_of(" \t");
	if(pos1 == std::string::npos) {
		str = "";
		return str;
	}
	if(pos1 == pos2) {
		str = str.substr(pos1);
		return str;
	}
	str = str.substr(pos1, pos2 - pos1 + 1);
	return str;
};

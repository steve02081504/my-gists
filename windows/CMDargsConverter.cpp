#pragma once
#include <string>
#include "../STL/replace_all.cpp"

std::string CMDargsConverter(std::string arg) {
	return '\"' + replace_all(arg, "\"", "\"\"") + '\"';
}

std::wstring CMDargsConverter(std::wstring arg) {
	return L'\"' + replace_all(arg, L"\"", L"\"\"") + L'\"';
}

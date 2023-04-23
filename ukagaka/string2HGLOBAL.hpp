#pragma once
#include <string_view>
#include <Windows.h>

struct HGLOBAL_t{
	HGLOBAL p;
	size_t size;
};
HGLOBAL_t string2HGLOBAL(const std::string_view a)noexcept;
HGLOBAL_t string2HGLOBAL(const std::wstring_view a,unsigned int CodePage=0/*CP_ACP*/);
std::string HGLOBAL2string(HGLOBAL_t a);

#pragma once
#include <Windows.h>
#include <cstring>
#include <string_view>
#include "string2HGLOBAL.hpp"
#include "../codepage.hpp"

HGLOBAL_t string2HGLOBAL(const std::string_view a){
	auto s=a.size();
	HGLOBAL p=GlobalAlloc(GMEM_FIXED,s);
	std::memcpy((void*)p,a.data(),s);
	return{p,s};
};
HGLOBAL_t string2HGLOBAL(const std::wstring_view a,unsigned int CodePage){
	return string2HGLOBAL(CODEPAGE_n::UnicodeToMultiByte(a.data(),CodePage));
}

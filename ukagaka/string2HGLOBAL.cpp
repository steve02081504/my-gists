#pragma once
#include <Windows.h>
#include <cstring>
#include <string_view>
#include "string2HGLOBAL.hpp"
#include "../codepage.hpp"

HGLOBAL_t string2HGLOBAL(const std::string_view a){
	auto s=a.size();
	HGLOBAL p=GlobalAlloc(GMEM_FIXED,s);
	if(p)
		std::memcpy((void*)p,a.data(),s);
	else
		s=0;
	return{p,s};
};
HGLOBAL_t string2HGLOBAL(const std::wstring_view a,unsigned int CodePage){
	return string2HGLOBAL(CODEPAGE_n::UnicodeToMultiByte(a.data(),CodePage));
}
std::string HGLOBAL2string(HGLOBAL_t a) {
	std::string b;
	if(a.p&&a.size)
		b=std::string((char*)a.p,a.size);
	GlobalFree(a.p);
	return b;
}

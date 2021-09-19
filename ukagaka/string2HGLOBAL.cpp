#include <Windows.h>
#include <cstring>
#include <string_view>
#include "../codepage.hpp"

struct HGLOBAL_t{
	HGLOBAL p;
	size_t size;
};
HGLOBAL_t string2HGLOBAL(const string_view a){
	auto s=a.size();
	HGLOBAL p=GlobalAlloc(GMEM_FIXED,s);
	std::memcpy((void*)p,a.c_str(),s);
	return{p,s}
};
HGLOBAL_t string2HGLOBAL(const wstring_view a,unsigned int CodePage=0/*CP_ACP*/){
	return string2HGLOBAL(CODEPAGE_n::UnicodeToMultiByte(a,CodePage));
}

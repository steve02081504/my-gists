#include <Windows.h>
#include <string_view>

struct HGLOBAL_t{
	HGLOBAL p;
	size_t size;
};
HGLOBAL_t string2HGLOBAL(const std::string_view a);
HGLOBAL_t string2HGLOBAL(const std::wstring_view a,unsigned int CodePage=0/*CP_ACP*/);

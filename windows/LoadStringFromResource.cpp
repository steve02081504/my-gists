#include "LoadStringFromResource.hpp"
#include <windows.h>

#if defined(RESOURCE_STRINGS_ARE_NULL_TERMINATED)
const wchar_t *LoadCStringFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance) noexcept {
	WCHAR *pBuf = NULL;

	const int len = LoadStringW(
		instance,
		stringID,
		reinterpret_cast<LPWSTR>(&pBuf), 0);

	if(len)
		return pBuf;
	else
		return L"";
}
#endif
std::wstring LoadStringFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance) {
	WCHAR *pBuf = NULL;

	int len = LoadStringW(
		instance,
		stringID,
		reinterpret_cast<LPWSTR>(&pBuf), 0);

	if(len)
		return std::wstring(pBuf, pBuf + len
		#if defined(RESOURCE_STRINGS_ARE_NULL_TERMINATED)
			-1//exclude null terminator
		#endif
		);
	else
		return L"";
}

std::wstring_view LoadStringViewFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance) noexcept {
	WCHAR *pBuf = NULL;

	const int len = LoadStringW(
		instance,
		stringID,
		reinterpret_cast<LPWSTR>(&pBuf), 0);

	if(len)
		return std::wstring_view(pBuf, len
		#if defined(RESOURCE_STRINGS_ARE_NULL_TERMINATED)
			-1//exclude null terminator
		#endif
		);
	else
		return L"";
}

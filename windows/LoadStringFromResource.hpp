#pragma once
#include <wtypes.h>
#include <string>
#include <string_view>
#if defined(RESOURCE_STRINGS_ARE_NULL_TERMINATED)
const wchar_t *LoadCStringFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance = NULL);
#endif
std::wstring LoadStringFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance = NULL);
std::wstring_view LoadStringViewFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance = NULL);

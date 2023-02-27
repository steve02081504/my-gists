#pragma once
#include <wtypes.h>
#include <string>
#if defined(RESOURCE_STRINGS_ARE_NULL_TERMINATED)
const wchar_t *LoadCStringFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance = NULL);
#endif
std::wstring LoadStringFromResource(
	__in UINT		   stringID,
	__in_opt HINSTANCE instance = NULL);

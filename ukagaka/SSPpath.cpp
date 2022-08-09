#include <windows.h>
#include "SSPpath.hpp"
#include "../windows/CMDargsConverter.cpp"

std::wstring GetSSPpath() {
	// Get the path to the SSP folder at reg HKEY_CLASSES_ROOT\x-ukagaka-link\shell\open\command
	HKEY hKey;
	LONG lResult = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"x-ukagaka-link\\shell\\open\\command", 0, KEY_READ, &hKey);
	if(lResult != ERROR_SUCCESS) {
		return L"";
	}
	DWORD	dwType;
	DWORD	dwSize = MAX_PATH;
	wchar_t szValue[MAX_PATH];
	lResult = RegQueryValueExW(hKey, NULL, NULL, &dwType, (LPBYTE)szValue, &dwSize);
	RegCloseKey(hKey);
	if(lResult != ERROR_SUCCESS) {
		return L"";
	}
	//szValue like "E:\ssp\ssp.exe" /M "%1"
	std::wstring sspPath = szValue;
	auto		 pos	 = sspPath.find_last_of(L"\" /M \"%1\"");
	if(pos == std::wstring::npos) {
		return L"";
	}
	sspPath = sspPath.substr(1, pos - 9);
	return sspPath;
}

bool IsSSPinstalled(std::wstring sspPath) {
	if(sspPath.empty()) {
		return false;
	}
	return _waccess(sspPath.c_str(), 0) == 0;
}

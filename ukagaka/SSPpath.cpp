#include <windows.h>
#include "SSPpath.hpp"
#include "../windows/CMDargsConverter.cpp"

std::wstring GetSSPpath() {
	// Get the path to the SSP folder at reg HKEY_CLASSES_ROOT\x-ukagaka-link\shell\open\command
	HKEY hKey;
	LONG lResult = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"x-ukagaka-link\\shell\\open\\command", 0, KEY_READ, &hKey);
	if(lResult != ERROR_SUCCESS) {
		//serch for the SSP path in the registry HKEY_CLASSES_ROOT\Local Settings\Software\Microsoft\Windows\Shell\MuiCache
		lResult = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache", 0, KEY_READ, &hKey);
		if(lResult != ERROR_SUCCESS)
			return {};
		//for each value in the registry key
		DWORD		 dwIndex = 0;
		LONG		 result;
		DWORD		 maxValueNameSize, maxValueDataSize;
		DWORD		 valueNameSize, valueDataSize, valueType;
		result = RegQueryInfoKeyW(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
		if(result != ERROR_SUCCESS)
			return {};
		std::wstring valueName(maxValueNameSize, L'\0');
		std::wstring valueData(maxValueDataSize, L'\0');
		while(true) {
			valueNameSize = maxValueNameSize;
			valueDataSize = maxValueDataSize;
			result		  = RegEnumValueW(hKey, dwIndex, valueName.data(), &valueNameSize, 0, &valueType, (BYTE*)valueData.data(), &valueDataSize);
			if(result != ERROR_SUCCESS)
				break;
			if(valueType == REG_SZ) {
				valueData.resize(lstrlenW(valueData.data()));
				//if the value data is "SSP"
				if(valueData == L"SSP") {
					//and if value name end with ".FriendlyAppName"
					valueName.resize(lstrlenW(valueName.data()));
					if(valueName.ends_with(L".FriendlyAppName")) {
						//close all
						RegCloseKey(hKey);
						//return the value name without the ".FriendlyAppName" if file exists
						auto filename = valueName.substr(0, valueName.size() - 16);
						if(_waccess(filename.c_str(), 0) == 0)
							return filename;
					}
					valueName.resize(maxValueNameSize);
				}
				valueData.resize(maxValueDataSize);
			}
			++dwIndex;
		}
		//close all
		RegCloseKey(hKey);
		//return empty string
		return {};
	}
	else {
		DWORD	dwType;
		DWORD	dwSize = MAX_PATH;
		wchar_t szValue[MAX_PATH];
		lResult = RegQueryValueExW(hKey, NULL, NULL, &dwType, (LPBYTE)szValue, &dwSize);
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS)
			return {};
		//szValue like "E:\ssp\ssp.exe" /M "%1"
		std::wstring sspPath = szValue;
		auto		 pos	 = sspPath.find_last_of(L"\" /M \"%1\"");
		if(pos == std::wstring::npos)
			return {};
		sspPath = sspPath.substr(1, pos - 9);
		return sspPath;
	}
}

bool IsSSPinstalled(std::wstring sspPath) {
	if(sspPath.empty())
		return false;
	return _waccess(sspPath.c_str(), 0) == 0;
}

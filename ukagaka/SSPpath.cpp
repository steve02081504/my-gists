#include <windows.h>
#include "SSPpath.hpp"
#include "../windows/CMDargsConverter.cpp"
#include "../codepage.hpp"

std::wstring GetSSPpath() {
	{
		//自ssp 2.6.14，查找临时文件夹下的ssp_path_tmp.txt内容
		wchar_t tmp_path[MAX_PATH];
		GetTempPathW(MAX_PATH, tmp_path);
		std::wstring ssp_path_tmp = tmp_path;
		ssp_path_tmp += L"ssp_path_tmp.txt";
		//用winapi读取文件内容
		HANDLE hFile = CreateFileW(ssp_path_tmp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			goto serch_in_reg;
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize == INVALID_FILE_SIZE || !dwFileSize) {
			CloseHandle(hFile);
			goto serch_in_reg;
		}
		std::string ssp_path_tmp_str;
		ssp_path_tmp_str.resize(dwFileSize);
		DWORD dwRead;
		if(!ReadFile(hFile, ssp_path_tmp_str.data(), dwFileSize, &dwRead, NULL)) {
			CloseHandle(hFile);
			goto serch_in_reg;
		}
		CloseHandle(hFile);
		//转换为wchar
		std::wstring ssp_path_tmp_wstr = CODEPAGE_n::MultiByteToUnicode(ssp_path_tmp_str, CODEPAGE_n::CP_UTF8);
		return ssp_path_tmp_wstr;
	}
	{
	serch_in_reg:
		// Get the path to the SSP folder at reg HKEY_CLASSES_ROOT\x-ukagaka-link\shell\open\command
		HKEY hKey;
		LONG lResult = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"x-ukagaka-link\\shell\\open\\command", 0, KEY_READ, &hKey);
		if(lResult != ERROR_SUCCESS) {
		serch_in_mui_cache:
			//serch for the SSP path in the registry HKEY_CLASSES_ROOT\Local Settings\Software\Microsoft\Windows\Shell\MuiCache
			lResult = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache", 0, KEY_READ, &hKey);
			if(lResult != ERROR_SUCCESS)
				return {};
			//for each value in the registry key
			DWORD dwIndex = 0;
			LONG  result;
			DWORD maxValueNameSize, maxValueDataSize;
			DWORD valueNameSize, valueDataSize, valueType;
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
				goto serch_in_mui_cache;
			//szValue like "E:\ssp\ssp.exe" /M "%1"
			std::wstring sspPath = szValue;
			auto		 pos	 = sspPath.find_last_of(L"\" /M \"%1\"");
			if(pos == std::wstring::npos)
				goto serch_in_mui_cache;
			sspPath = sspPath.substr(1, pos - 9);
			if(_waccess(sspPath.c_str(), 0) == 0)
				return sspPath;
			else
				goto serch_in_mui_cache;
		}
	}
}

bool IsSSPinstalled(std::wstring sspPath) {
	if(sspPath.empty())
		return false;
	return _waccess(sspPath.c_str(), 0) == 0;
}

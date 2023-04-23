#pragma once
#include <windows.h>

struct ICON_INFO_t{
	HICON hIcon, hIconSmall;
};
BOOL GetIcon(HWND hWnd, ICON_INFO_t *pIconInfo);
BOOL SetIcon(HWND hWnd, ICON_INFO_t IconInfo);

HICON LoadIconFrom(LPCWSTR pszFileName);
HICON LoadIconFrom(std::wstring icon_path);
HICON LoadIconWithBasePath(std::wstring base_path, std::wstring icon_path);

void SetIcon(HWND hWnd, HICON hIcon);
bool SetIcon(HWND hWnd, LPCWSTR pszFileName);
bool SetIcon(HWND hWnd, std::wstring icon_path);
ICON_INFO_t GetIcon(HWND hWnd);

ICON_INFO_t GetConsoleIcon();
void SetConsoleIcon(HICON hIcon);
bool SetConsoleIcon(LPCWSTR pszFileName);
bool SetConsoleIcon(std::wstring icon_path);
bool SetConsoleIcon(ICON_INFO_t icon_info);

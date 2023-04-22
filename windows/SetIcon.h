#pragma once
#include <windows.h>

struct ICON_INFO_t{
	HICON hIcon, hIconSmall;
};
BOOL GetIcon(HWND hWnd, ICON_INFO_t *pIconInfo);
BOOL SetIcon(HWND hWnd, ICON_INFO_t IconInfo);

HICON LoadIconFrom(LPCWSTR pszFileName);

void SetIcon(HWND hWnd, HICON hIcon);
bool SetIcon(HWND hWnd, LPCWSTR pszFileName);
ICON_INFO_t GetIcon(HWND hWnd);

ICON_INFO_t GetConsoleIcon();
void SetConsoleIcon(HICON hIcon);
bool SetConsoleIcon(LPCWSTR pszFileName);
bool SetConsoleIcon(ICON_INFO_t icon_info);

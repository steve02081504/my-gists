#pragma once
#include <windows.h>

struct ICON_INFO_t{
	HICON hIcon, hIconSmall;
};
BOOL GetIcon(HWND hWnd, ICON_INFO_t *pIconInfo) noexcept;
BOOL SetIcon(HWND hWnd, ICON_INFO_t IconInfo) noexcept;

HICON LoadIconFrom(LPCWSTR pszFileName) noexcept;
HICON LoadIconFrom(std::wstring icon_path) noexcept;
HICON LoadIconWithBasePath(std::wstring base_path, std::wstring icon_path);

void SetIcon(HWND hWnd, HICON hIcon)noexcept;
bool SetIcon(HWND hWnd, LPCWSTR pszFileName)noexcept;
bool SetIcon(HWND hWnd, std::wstring icon_path)noexcept;
ICON_INFO_t GetIcon(HWND hWnd) noexcept;

ICON_INFO_t GetConsoleIcon()noexcept;
void SetConsoleIcon(HICON hIcon)noexcept;
bool SetConsoleIcon(LPCWSTR pszFileName)noexcept;
bool SetConsoleIcon(std::wstring icon_path)noexcept;
bool SetConsoleIcon(ICON_INFO_t icon_info)noexcept;

#pragma once
#include <windows.h>
#include "SetIcon.h"

BOOL GetIcon(HWND hWnd, ICON_INFO_t *pIconInfo) {
	if(hWnd == NULL)
		return FALSE;

	pIconInfo->hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_BIG, 0);
	pIconInfo->hIconSmall = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0);
	return TRUE;
}
BOOL SetIcon(HWND hWnd, ICON_INFO_t IconInfo) {
	if(hWnd == NULL)
		return FALSE;

	if(IconInfo.hIcon)
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)IconInfo.hIcon);
	if(IconInfo.hIconSmall)
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)IconInfo.hIconSmall);
	return TRUE;
}

void SetIcon(HWND hWnd, HICON hIcon) {
	SetIcon(hWnd, {hIcon, hIcon});
}
HICON LoadIconFrom(LPCWSTR pszFileName) {
	return (HICON)LoadImageW(
		NULL,
		pszFileName,
		IMAGE_ICON,
		0, 0,
		LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
}
bool SetIcon(HWND hWnd, LPCWSTR pszFileName) {
	HICON hIcon = LoadIconFrom(pszFileName);
	if(hIcon)
		SetIcon(hWnd, hIcon);
	return hIcon;
}
ICON_INFO_t GetIcon(HWND hWnd) {
	ICON_INFO_t icon_info;
	GetIcon(hWnd, &icon_info);
	return icon_info;
}
ICON_INFO_t GetConsoleIcon() {
	return GetIcon(GetConsoleWindow());
}
void SetConsoleIcon(HICON hIcon) {
	SetIcon(GetConsoleWindow(), hIcon);
}
bool SetConsoleIcon(LPCWSTR pszFileName) {
	return SetIcon(GetConsoleWindow(), pszFileName);
}
bool SetConsoleIcon(ICON_INFO_t icon_info) {
	return SetIcon(GetConsoleWindow(), icon_info);
}

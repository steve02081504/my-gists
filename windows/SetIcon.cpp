#include <windows.h>
#include "SetIcon.h"

void SetIcon(HWND hWnd, HICON hIcon) {
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}
void SetIcon(HWND hWnd, LPCWSTR pszFileName) {
	HICON hIcon = (HICON)LoadImageW(
		NULL, 
		pszFileName,
		IMAGE_ICON,
		0, 0,
		LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE
	);
	SetIcon(hWnd,hIcon);
}



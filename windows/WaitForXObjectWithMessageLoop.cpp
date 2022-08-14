#pragma once
#include "WaitForXObjectWithMessageLoop.hpp"
#include <windows.h>

DWORD WaitForSingleObjectWithMessageLoop(HANDLE hHandle, DWORD dwMilliseconds) {
	MSG msg;
	while(1) {
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {
			auto msec_wait = min(100, dwMilliseconds);
			if(dwMilliseconds != INFINITE)
				dwMilliseconds -= msec_wait;
			DWORD dwRet = ::WaitForSingleObject(hHandle, msec_wait);
			if(dwRet == WAIT_TIMEOUT) {
				if(dwMilliseconds == 0)
					return dwRet;
			}
			else
				return dwRet;
		}
	}
	return msg.wParam;
}

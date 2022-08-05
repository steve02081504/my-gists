#include <windows.h>

inline const OSVERSIONINFO OSinfo;

inline bool IsWinNT() {
	return (OSinfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

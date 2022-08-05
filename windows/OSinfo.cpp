#include <windows.h>
#include <string>

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

inline OSVERSIONINFO OSinfo		  = {sizeof(OSinfo)};
static bool			 OSinfoIniter = GetVersionEx(&OSinfo);

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

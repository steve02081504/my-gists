#include <windows.h>
#include "FILETIME2time_t.h"

time_t FILETIME2time_t(const FILETIME ft){
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&ft,&localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime,&sysTime);
	struct tm tmtime = {0};
	tmtime.tm_year = sysTime.wYear - 1900;
	tmtime.tm_mon = sysTime.wMonth - 1;
	tmtime.tm_mday = sysTime.wDay;
	tmtime.tm_hour = sysTime.wHour;
	tmtime.tm_min = sysTime.wMinute;
	tmtime.tm_sec = sysTime.wSecond;
	tmtime.tm_wday = 0;
	tmtime.tm_yday = 0;
	tmtime.tm_isdst = -1;
	time_t ret = mktime(&tmtime);
	return ret;
}

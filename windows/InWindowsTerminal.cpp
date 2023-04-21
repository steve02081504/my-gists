#include <windows.h>
#include <tlhelp32.h>//PROCESSENTRY32W

bool InWindowsTerminal_helper(){
	//判断是否为Windows Terminal
	//依据为当前进程和WindowsTerminal.exe的进程在同一进程树下
	bool aret=0;
	auto pid = GetCurrentProcessId();
	while(pid) {
		wchar_t buf[MAX_PATH];
		auto	h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
		if(!h)
			break;
		DWORD len = MAX_PATH;
		if(!QueryFullProcessImageNameW(h, 0, buf, &len)) {
			CloseHandle(h);
			break;
		}
		CloseHandle(h);
		if(wcsstr(buf, L"WindowsTerminal.exe")) {
			aret = 1;
			break;
		}
		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32W);
		auto hSnap	= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(hSnap == INVALID_HANDLE_VALUE)
			break;
		if(!Process32FirstW(hSnap, &pe32)) {
			CloseHandle(hSnap);
			break;
		}
		do {
			if(pe32.th32ProcessID == pid) {
				pid = pe32.th32ParentProcessID;
				break;
			}
		} while(Process32NextW(hSnap, &pe32));
		CloseHandle(hSnap);
	}
	return aret;
}
bool InWindowsTerminal() {
	static bool aret = InWindowsTerminal_helper();
	return aret;
}

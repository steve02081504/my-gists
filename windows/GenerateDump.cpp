#include "Windows.h"
#include "DbgHelp.h"

void OnError(PEXCEPTION_POINTERS pExceptionPointers);

int GenerateDump(PEXCEPTION_POINTERS pExceptionPointers) noexcept {
	HANDLE hDumpFile = CreateFileW(L"Dump.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDumpFile == INVALID_HANDLE_VALUE) {
		return EXCEPTION_EXECUTE_HANDLER;
	}

	typedef BOOL(WINAPI * MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
											 CONST PMINIDUMP_EXCEPTION_INFORMATION	 ExceptionParam,
											 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
											 CONST PMINIDUMP_CALLBACK_INFORMATION	 CallbackParam);
	HMODULE hDbgHelp = LoadLibraryW(L"DbgHelp.dll");
	if(hDbgHelp == NULL) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = (MINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if(!pfnMiniDumpWriteDump) {
		return EXCEPTION_EXECUTE_HANDLER;
	}

	MINIDUMP_EXCEPTION_INFORMATION mdei{};
	mdei.ThreadId		   = GetCurrentThreadId();
	mdei.ExceptionPointers = pExceptionPointers;
	mdei.ClientPointers	   = FALSE;

	const BOOL bRet = pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &mdei, NULL, NULL);
	CloseHandle(hDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}
LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo) {
	if(IsDebuggerPresent()) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	const auto tmp = GenerateDump(lpExceptionInfo);
	OnError(lpExceptionInfo);
	return tmp;
}
void InstallExceptionFilter() noexcept {
	SetUnhandledExceptionFilter(ExceptionFilter);
}

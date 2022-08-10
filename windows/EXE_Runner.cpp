#include "../windows/CMDargsConverter.cpp"
#include "EXE_Runner.hpp"

bool EXE_Runner::Base_Run(std::wstring args) {
	if((INT_PTR)ShellExecuteW(NULL, L"open", _path.c_str(), args.c_str(), NULL, SW_SHOW) <= 32)
		return false;
	return true;
}
bool EXE_Runner::Base_RunAndWait(std::wstring args) {
	//Execute the program and wait for it to finish
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if(!CreateProcessW(_path.c_str(), (LPWSTR)args.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return false;
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

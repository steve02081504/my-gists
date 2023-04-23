#include "../windows/CMDargsConverter.cpp"
#include "../windows/WaitForXObjectWithMessageLoop.cpp"
#include "EXE_Runner.hpp"

bool EXE_Runner::Base_Run(std::wstring args){
	if((INT_PTR)ShellExecuteW(NULL, L"open", _path.c_str(), args.c_str(), NULL, SW_SHOW) <= 32)
		return false;
	return true;
}
bool EXE_Runner::Base_RunAndWait(std::wstring args) {
	//Execute the program and wait for it to finish
	STARTUPINFOW		si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	auto cmdline = L"\"" + _path + L"\" " + args;
	if(!CreateProcessW(NULL, cmdline.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return false;
	WaitForSingleObjectWithMessageLoop(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

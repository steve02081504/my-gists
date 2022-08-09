#include "../windows/CMDargsConverter.cpp"
#include "EXE_Runner.hpp"

bool EXE_Runner::Run(std::wstring args) {
	if((INT_PTR)ShellExecuteW(NULL, L"open", _path.c_str(), args.c_str(), NULL, SW_SHOW) <= 32)
		return false;
	return true;
}

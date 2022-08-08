#include "SSPpath.cpp"
#include "../windows/CMDargsConverter.cpp"
#include "SSP_Runner.hpp"

bool SSP_Runner::Run(std::wstring args) {
	if(!_installed)
		return false;
	if((INT_PTR)ShellExecuteW(NULL, L"open", _path.c_str(), args.c_str(), NULL, SW_SHOW) <= 32) {
		return false;
	}
	return true;
}

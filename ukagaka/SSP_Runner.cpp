#include "SSPpath.cpp"
#include "../windows/EXE_Runner.cpp"
#include "SSP_Runner.hpp"

bool SSP_Runner::Base_Run(const std::wstring& args) noexcept {
	if(!_installed)
		return false;
	return EXE_Runner::Base_Run(args);
}

bool SSP_Runner::Base_RunAndWait(const std::wstring& args) {
	if(!_installed)
		return false;
	return EXE_Runner::Base_RunAndWait(args);
}

#include "SSPpath.cpp"
#include "../windows/EXE_Runner.cpp"
#include "SSP_Runner.hpp"

bool SSP_Runner::Base_Run(std::wstring args) {
	if(!_installed)
		return false;
	return EXE_Runner::Base_Run(args);
}

bool SSP_Runner::Base_RunAndWait(std::wstring args) {
	if(!_installed)
		return false;
	return EXE_Runner::Base_RunAndWait(args);
}

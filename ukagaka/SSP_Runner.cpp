#include "SSPpath.cpp"
#include "../windows/EXE_Runner.cpp"
#include "SSP_Runner.hpp"

bool SSP_Runner::Run(std::wstring args) {
	if(!_installed)
		return false;
	return EXE_Runner::Run(args);
}

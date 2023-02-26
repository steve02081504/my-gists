#include <windows.h>
#include "EnableVirtualTerminal.hpp"

void EnableVirtualTerminal_base(HANDLE stdhandle)noexcept{
	if(stdhandle == INVALID_HANDLE_VALUE)
		return;

	DWORD dwMode = 0;
	if(!GetConsoleMode(stdhandle, &dwMode))
		return;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if(!SetConsoleMode(stdhandle, dwMode))
		return;
}
void EnableVirtualTerminal()noexcept{
	EnableVirtualTerminal_base(GetStdHandle(STD_OUTPUT_HANDLE));
	EnableVirtualTerminal_base(GetStdHandle(STD_ERROR_HANDLE));
}
void DisableVirtualTerminal_base(HANDLE stdhandle)noexcept {
	if(stdhandle == INVALID_HANDLE_VALUE)
		return;

	DWORD dwMode = 0;
	if(!GetConsoleMode(stdhandle, &dwMode))
		return;

	dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if(!SetConsoleMode(stdhandle, dwMode))
		return;
}
void DisableVirtualTerminal()noexcept {
	DisableVirtualTerminal_base(GetStdHandle(STD_OUTPUT_HANDLE));
	DisableVirtualTerminal_base(GetStdHandle(STD_ERROR_HANDLE));
}

#include <windows.h>

bool IsElevated_helper(){
	bool aret=0;
	HANDLE hToken = NULL;
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken)){
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if( GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
			aret = Elevation.TokenIsElevated;
	}
	if(hToken)
		CloseHandle(hToken);
	return aret;
}
bool IsElevated(){
	static bool aret = IsElevated_helper();
	return aret;
}

FILETIME LastWriteTime(const wchar_t* filename){
	HANDLE hFile = CreateFileW(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	FILETIME fWriteTime{};

	if (hFile != INVALID_HANDLE_VALUE) {
		GetFileTime(hFile, NULL, NULL, &fWriteTime);
		if (!fWriteTime.dwHighDateTime) {
			GetFileTime(hFile, NULL, &fWriteTime, NULL);
		}
		CloseHandle(hFile);
	}
	return fWriteTime;
}

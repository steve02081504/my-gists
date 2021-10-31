__int64 GetFileSize(const wchar_t* filename){
	HANDLE hFile = CreateFileW(filename,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

	LARGE_INTEGER size{};

	if (hFile != INVALID_HANDLE_VALUE)
		GetFileSizeEx(hFile, &size);
	return size.QuadPart;
}

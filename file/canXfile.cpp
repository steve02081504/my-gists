#pragma once
//若Windows，使用winapi来检查文件是否可读写
#ifdef _WIN32
	#include <Windows.h>
#else
	#define _CRT_SECURE_NO_WARNINGS
	#include <cstdio>
#endif
//const char
[[nodiscard]] bool can_read_file(const char* filename) noexcept {
	#ifdef _WIN32
		HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	#else
		FILE* f = fopen(filename, "r");
		if(f)
			fclose(f);
		return f != NULL;
	#endif
}

[[nodiscard]] bool can_write_file(const char* filename) noexcept {
	#ifdef _WIN32
		HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	#else
		FILE* f = fopen(filename, "w");
		if(f)
			fclose(f);
		return f != NULL;
	#endif
}

[[nodiscard]] bool can_read_write_file(const char* filename) noexcept {
	#ifdef _WIN32
		HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	#else
		FILE* f = fopen(filename, "r+");
		if(f)
			fclose(f);
		else {
			f = fopen(filename, "w+");
			if(f)
				fclose(f);
		}
		return f != NULL;
	#endif
}
//wconst char
[[nodiscard]] bool can_read_file(const wchar_t* filename) noexcept {
	#ifdef _WIN32
		HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	#else
		FILE* f = _wfopen(filename, L"r");
		if(f)
			fclose(f);
		return f != NULL;
	#endif
}

[[nodiscard]] bool can_write_file(const wchar_t* filename) noexcept {
	#ifdef _WIN32
		HANDLE hFile = CreateFileW(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	#else
		FILE* f = _wfopen(filename, L"w");
		if(f)
			fclose(f);
		return f != NULL;
	#endif
}

[[nodiscard]] bool can_read_write_file(const wchar_t* filename) noexcept {
	#ifdef _WIN32
		HANDLE hFile = CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	#else
		FILE* f = _wfopen(filename, L"r+");
		if(f)
			fclose(f);
		else {
			f = _wfopen(filename, L"w+");
			if(f)
				fclose(f);
		}
		return f != NULL;
	#endif
}

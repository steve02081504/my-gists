#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
//const char
[[nodiscard]] bool can_read_file(const char* filename) {
	FILE* f = fopen(filename, "r");
	if(f)
		fclose(f);
	return f != NULL;
}

[[nodiscard]] bool can_write_file(const char* filename) {
	FILE* f = fopen(filename, "w");
	if(f)
		fclose(f);
	return f != NULL;
}

[[nodiscard]] bool can_read_write_file(const char* filename) {
	FILE* f = fopen(filename, "r+");
	if(f)
		fclose(f);
	else {
		f = fopen(filename, "w+");
		if(f)
			fclose(f);
	}
	return f != NULL;
}
//wconst char
[[nodiscard]] bool can_read_file(const wchar_t* filename) {
	FILE* f = _wfopen(filename, L"r");
	if(f)
		fclose(f);
	return f != NULL;
}

[[nodiscard]] bool can_write_file(const wchar_t* filename) {
	FILE* f = _wfopen(filename, L"w");
	if(f)
		fclose(f);
	return f != NULL;
}

[[nodiscard]] bool can_read_write_file(const wchar_t* filename) {
	FILE* f = _wfopen(filename, L"r+");
	if(f)
		fclose(f);
	else {
		f = _wfopen(filename, L"w+");
		if(f)
			fclose(f);
	}
	return f != NULL;
}

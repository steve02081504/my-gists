// 
// AYA version 5
//
// ディレクトリ内列挙　CDirEnum
// 
#include <windows.h>
#include <list>
#include <algorithm>

#include "../codepage.hpp"
#include "dir_enum.h"

CDirEnum::CDirEnum(const std::wstring&ep)
{
	enumpath = ep;
	is_init = false;
}

CDirEnum::~CDirEnum()
{
	if( is_init ) {
#if defined(WIN32)
		::FindClose(dh);
#elif defined(POSIX)
		closedir(dh);
#endif
	}
}

#if defined(WIN32) || defined(_WIN32_WCE)
#pragma warning(disable : 4996)
class IsUnicodeAwareHelper
{
public:
	bool isnt;

	IsUnicodeAwareHelper() {
		OSVERSIONINFO osVer;
		osVer.dwOSVersionInfoSize = sizeof(osVer);

		::GetVersionEx(&osVer);
		isnt = (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT);
	}
};

bool	IsUnicodeAware(void)
{
	static IsUnicodeAwareHelper h;
	return h.isnt;
}
#else
bool	IsUnicodeAware(void)
{
	return true;
}
#endif

bool CDirEnum::next(CDirEnumEntry &entry)
{
	std::string name_a;
	std::wstring name_w;
	bool isdir = false;

	bool isUnicode = false;

#if defined(WIN32)
	isUnicode = IsUnicodeAware();
#endif

	while ( true ) {
		if( ! is_init ) {
#if defined(WIN32)
			std::wstring tmp_str = enumpath + L"\\*.*";

			if( isUnicode ) {
				WIN32_FIND_DATAW w32FindData;

				dh = ::FindFirstFileW(tmp_str.c_str(),&w32FindData);

				if( dh == INVALID_HANDLE_VALUE ) { return false; }

				is_init = true;

				name_w = w32FindData.cFileName;
				isdir = (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			}
			else {
				WIN32_FIND_DATAA w32FindData;

				std::string s_filestr = CODEPAGE_n::UnicodeToMultiByte(tmp_str, CODEPAGE_n::CP_ACP);
				if( s_filestr.empty() ) { return false; }

				dh = ::FindFirstFileA(s_filestr.c_str(),&w32FindData);

				if( dh == INVALID_HANDLE_VALUE ) { return false; }

				is_init = true;

				name_a = w32FindData.cFileName;
				isdir = (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			}

#elif defined(POSIX)

			std::string path = narrow(enumpath);
			fix_filepath(path);

			dh = opendir(path.c_str());
			if( ! dh ) { return false; }

			struct dirent* ent = readdir(dh);
			if( ! ent ) { closedir(dh); return false; }

			is_init = true;
	
			name_a = ent->d_name;
			isdir = ent->d_type == DT_DIR;

#endif
		}
		else {
#if defined(WIN32)

			if( isUnicode ) {
				WIN32_FIND_DATAW w32FindData;
				if( ::FindNextFileW(dh,&w32FindData) == 0 ) { return false; }

				name_w = w32FindData.cFileName;
				isdir = (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			}
			else {
				WIN32_FIND_DATAA w32FindData;
				if( ::FindNextFileA(dh,&w32FindData) == 0 ) { return false; }

				name_a = w32FindData.cFileName;
				isdir = (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			}

#elif defined(POSIX)

			struct dirent* ent = readdir(dh);
			if( ! ent ) { return false; }

			name_a = ent->d_name;
			isdir = ent->d_type == DT_DIR;

#endif
		}

		if( isUnicode ) {
			if(name_w != L"." && name_w != L"..") {
				break;
			}
		}
		else {
			if(name_a != "." && name_a != "..") {
				break;
			}
		}
	}

	if( isUnicode ) {
		entry.name = name_w;
	}
	else {
		std::wstring t_wfile = CODEPAGE_n::MultiByteToUnicode(name_a, CODEPAGE_n::CP_ACP);
		if( t_wfile.empty() ) { return false; }

		entry.name = t_wfile;
	}
#if defined(POSIX)
	entry.name = widen(name_a);
#endif

	entry.isdir = isdir;

	return true;
}



// 
// AYA version 5
//
// ディレクトリ内列挙　CDirEnum
// 

#ifndef	DIR_ENUM_H
#define	DIR_ENUM_H

#include <stdio.h>
#include <list>

#if defined(POSIX)
# include <dirent.h>
#endif


//----

class CDirEnumEntry
{
public:
	std::wstring name;
	bool isdir;
};

class CDirEnum {
private:

#if defined(WIN32)
	HANDLE dh;
#elif defined(POSIX)
	DIR* dh;
#endif
	bool is_init;
	std::wstring enumpath;

public:
	CDirEnum(const std::wstring&enumpath);
	~CDirEnum();

	bool next(CDirEnumEntry &entry);
};

#endif // DIR_ENUM_H

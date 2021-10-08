#pragma once
#include <Windows.h>
#include "string2HGLOBAL.hpp"
#include "../file/GetFilename_sPath.hpp"
#include "../codepage.hpp"
class Cshiori {
	HINSTANCE dll = NULL;

	typedef BOOL __cdecl load_type_t(HGLOBAL h, long len);
	typedef load_type_t* load_type;
	typedef BOOL __cdecl unload_type_t();
	typedef unload_type_t* unload_type;
	typedef HGLOBAL __cdecl request_type_t(HGLOBAL h, long* len);
	typedef request_type_t* request_type;

	typedef BOOL __cdecl CI_check_t(void);
	typedef CI_check_t* CI_check_type;

	std::wstring filename;
	load_type load=NULL;
	bool loadok=1;
	unload_type unload=NULL;
	request_type request=NULL;
	CI_check_type checker;
	CODEPAGE_n::CODEPAGE cp=CODEPAGE_n::CP_UTF8;

	void init_methods();
	void call_load(LPCWSTR pszFileName);
	void call_unload();
public:
	bool All_OK();
	Cshiori();
	void SetTo(LPCWSTR pszFileName);
	Cshiori(LPCWSTR pszFileName);
	~Cshiori();
	void Doreload();
	void Dounload();
	void SetCodePage(std::wstring);
	void SetCodePage(CODEPAGE_n::CODEPAGE);
	std::string operator()(std::string);
	std::wstring operator()(std::wstring);
	bool CI_check_failed();
	bool can_make_CI_check();
};

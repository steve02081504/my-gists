#pragma once
#include <Windows.h>
#include "string2HGLOBAL.hpp"
#include "../file/GetFilename_sPath.hpp"
class Cshiori {
	HINSTANCE dll = NULL;

	typedef BOOL __cdecl load_type_t(HGLOBAL h, long len);
	typedef load_type_t* load_type;
	typedef BOOL __cdecl unload_type_t();
	typedef unload_type_t* unload_type;
	typedef HGLOBAL __cdecl request_type_t(HGLOBAL h, long* len);
	typedef request_type_t* request_type;

	std::wstring filename;
	load_type load=NULL;
	bool loadok=1;
	unload_type unload=NULL;
	request_type request=NULL;

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
};

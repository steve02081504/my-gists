#include <Windows.h>
#include "string2HGLOBAL.hpp"
#include "../file/GetFilename_sPath.hpp"
struct Cshiori{
	HINSTANCE dll=NULL;

	typedef BOOL __cdecl load_type_t(HGLOBAL h, long len);
	typedef load_type_t* load_type;
	typedef BOOL __cdecl unload_type_t();
	typedef unload_type_t* unload_type;
	typedef HGLOBAL __cdecl request_type_t(HGLOBAL h, long* len);
	typedef request_type_t* request_type;

	load_type load=NULL;
	bool loadok=1;
	unload_type unload=NULL;
	request_type request=NULL;

	bool All_OK(){return dll&&load&&unload&&loadok&&request;}

	void init_methods(){
		load=(load_type)GetProcAddress(dll,"load");
		unload=(unload_type)GetProcAddress(dll,"unload");
		request=(request_type)GetProcAddress(dll,"request");
	}
	void call_load(LPCWSTR pszFileName){
		auto a=string2HGLOBAL(GetFilename_sPath(pszFileName));
		loadok=load(a.p,a.size);
		if(!loadok)
			this->~Cshiori();
	}
	void call_unload(){
		if(unload&&loadok)
			unload();//no body fucking cares returns
	}
	Cshiori(){}
	void SetTo(LPCWSTR pszFileName){
		dll=LoadLibrary(pszFileName);
		init_methods();
		if(All_OK())
			call_load(pszFileName);
		else
			this->~Cshiori();
	}
	Cshiori(LPCWSTR pszFileName){
		SetTo(pszFileName);
	}
	~Cshiori(){
		call_unload();
		FreeLibrary(dll);
	}
};

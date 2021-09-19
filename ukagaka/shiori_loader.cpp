#pragma once
#include <Windows.h>
#include "string2HGLOBAL.hpp"
#include "shiori_loader.hpp"
#include "../file/GetFilename_sPath.hpp"

bool Cshiori::All_OK(){return dll&&load&&unload&&loadok&&request;}

void Cshiori::init_methods(){
	load=(load_type)GetProcAddress(dll,"load");
	unload=(unload_type)GetProcAddress(dll,"unload");
	request=(request_type)GetProcAddress(dll,"request");
}
void Cshiori::call_load(LPCWSTR pszFileName){
	loadok=1;
	auto a=string2HGLOBAL(GetFilename_sPath(pszFileName));
	loadok=load(a.p,a.size);
	if(!loadok)
		this->~Cshiori();
}
void Cshiori::call_unload(){
	if(unload&&loadok)
		unload();//no body fucking cares returns
}
Cshiori::Cshiori(){}
void Cshiori::SetTo(LPCWSTR pszFileName){
	if(dll)
		this->~Cshiori();
	dll=LoadLibrary(pszFileName);
	init_methods();
	if(All_OK())
		call_load(pszFileName);
	else
		this->~Cshiori();
}
Cshiori::Cshiori(LPCWSTR pszFileName){
	SetTo(pszFileName);
}
Cshiori::~Cshiori(){
	call_unload();
	FreeLibrary(dll);
	dll=NULL;
}

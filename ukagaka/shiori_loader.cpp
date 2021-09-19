#pragma once
#include <Windows.h>
#include "string2HGLOBAL.cpp"
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
		Dounload();
	filename=pszFileName;
	dll=LoadLibrary(pszFileName);
	init_methods();
	if(All_OK())
		call_load(pszFileName);
	else
		Dounload();
}
Cshiori::Cshiori(LPCWSTR pszFileName){
	SetTo(pszFileName);
}
Cshiori::~Cshiori(){
	Dounload();
}
void Cshiori::Doreload(){
	this->~Cshiori();
	SetTo(filename.c_str());
}
void Cshiori::Dounload(){
	call_unload();
	unload=NULL;
	if(dll)
		FreeLibrary(dll);
	dll=NULL;
}

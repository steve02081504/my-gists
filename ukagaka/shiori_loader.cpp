#pragma once
#include <Windows.h>
#include "string2HGLOBAL.cpp"
#include "shiori_loader.hpp"
#include "../file/GetFilename_sPath.hpp"
#include "../codepage.cpp"

bool Cshiori::All_OK(){return dll&&load&&unload&&loadok&&request;}

void Cshiori::init_methods(){
	load=(load_type)GetProcAddress(dll,"load");
	unload=(unload_type)GetProcAddress(dll,"unload");
	request=(request_type)GetProcAddress(dll,"request");
	checker=(CI_check_type)GetProcAddress(dll,"CI_check_failed");
}
void Cshiori::call_load(LPCWSTR pszFileName){
	loadok=1;
	auto a=string2HGLOBAL(GetFilename_sPath(pszFileName));
	loadok=load(a.p,a.size);
	if(!loadok)
		Dounload();
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
	Dounload();
	SetTo(filename.c_str());
}
void Cshiori::Dounload(){
	call_unload();
	unload=NULL;
	if(dll)
		FreeLibrary(dll);
	dll=NULL;
}
using namespace CODEPAGE_n;
void Cshiori::SetCodePage(std::wstring a){
	SetCodePage((CODEPAGE_n::CODEPAGE)StringtoCodePage(a.c_str()));
}

void Cshiori::SetCodePage(CODEPAGE_n::CODEPAGE a){
	cp=a;
}

std::string Cshiori::operator()(std::string a)
{
	if (request){
		auto b=string2HGLOBAL(a);
		long c = b.size;
		auto d=request(b.p,&c);
		return HGLOBAL2string({d,(size_t)c});
	}
	else
		return std::string();
}

std::wstring Cshiori::operator()(std::wstring a){
	return MultiByteToUnicode(operator()(UnicodeToMultiByte(a,cp)),cp);
}

bool Cshiori::CI_check_failed(){
	return (!checker)||checker();
}

bool Cshiori::can_make_CI_check(){
	return checker;
}

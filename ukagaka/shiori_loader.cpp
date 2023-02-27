#pragma once
#include <Windows.h>
#include "string2HGLOBAL.cpp"
#include "shiori_loader.hpp"
#include "../file/GetFilename_sPath.hpp"
#include "../codepage.cpp"
#include "../ansi_color.hpp"

bool Cshiori::All_OK() { return methods_All_OK() && loadok; }
bool Cshiori::methods_All_OK() { return dll && load && unload && request; }

void Cshiori::init_methods(){
	load=(load_type)GetProcAddress(dll,"load");
	if(!load)
		error_logger(Error::interface_load_not_found);
	unload=(unload_type)GetProcAddress(dll,"unload");
	if(!unload)
		error_logger(Error::interface_unload_not_found);
	request=(request_type)GetProcAddress(dll,"request");
	if(!request)
		error_logger(Error::interface_request_not_found);

	checker=(CI_check_type)GetProcAddress(dll,"CI_check_failed");
	if(!checker)
		error_logger(Warning::interface_CI_check_not_found);
	logsender=(logsender_type)GetProcAddress(dll,"logsend");
	if(!logsender)
		error_logger(Warning::interface_logsend_not_found);
}
void Cshiori::call_load(LPCWSTR pszFileName){
	auto a=string2HGLOBAL(GetFilename_sPath(pszFileName));
	loadok=load(a.p,a.size);
	if(!loadok) {
		error_logger(Error::interface_load_failed);
		Dounload();
	}
}
bool Cshiori::call_unload(){
	if(!loadok)
		error_logger(Error::skip_unload_call_because_load_failed);
	else if(!unload)
		error_logger(Error::skip_unload_call_because_unload_not_found);
	else{
		const auto aret = unload();
		if(!aret)
			error_logger(Error::interface_unload_failed);
		return aret;
	}
	return false;
}
Cshiori::Cshiori(error_logger_type error_logger_v) {
	error_logger = error_logger_v;
}
void Cshiori::SetTo(LPCWSTR pszFileName){
	if(dll)
		Dounload();
	loadok=false;
	filename=pszFileName;
	dll=LoadLibraryW(pszFileName);
	if(!dll)
		error_logger(Error::dll_file_load_failed);
	else
		init_methods();
	if(methods_All_OK()) {
		if(loghandler)
			Set_loghandler(loghandler);
		if(hwnd_for_logsender) {
			set_logsend_ok = set_logsend(hwnd_for_logsender);
			if(!set_logsend_ok)
				error_logger(Warning::logsend_failed);
		}
		call_load(pszFileName);
	}
	else
		Dounload();
}
Cshiori::Cshiori(LPCWSTR pszFileName,error_logger_type error_logger_v):Cshiori(error_logger_v){
	SetTo(pszFileName);
}
Cshiori::~Cshiori(){
	Dounload();
}
void Cshiori::Doreload(){
	Dounload();
	SetTo(filename.c_str());
}
bool Cshiori::Dounload(){
	if(!dll)return true;//?
	const bool aret=call_unload();
	unload=NULL;
	if(dll)
		FreeLibrary(dll);
	dll=NULL;
	return aret;
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

bool Cshiori::can_set_logsend(){
	return logsender;
}

bool Cshiori::set_logsend(HWND hwnd) {
	return (!logsender)||logsender((long)hwnd);
}

void Cshiori::set_logsend_hwnd(HWND hwnd) {
	hwnd_for_logsender=hwnd;
}

bool Cshiori::is_logsend_ok() {
	return set_logsend_ok;
}

void Cshiori::Set_loghandler(void (*loghandler_v)(const wchar_t *str, int mode, int id)){
	loghandler=loghandler_v;
	auto Setter=(Set_loghandler_type)GetProcAddress(dll,"Set_loghandler");
	if(Setter)
		Setter(loghandler);
}

std::string_view to_string(Cshiori::Error err) {
	using enum Cshiori::Error;
	switch(err) {
	case interface_load_not_found:
		return "Necessary interface \"load\" not found";
	case interface_unload_not_found:
		return "Necessary interface \"unload\" not found";
	case interface_request_not_found:
		return "Necessary interface \"request\" not found";
	case interface_load_failed:
		return "Failed to load, unloading file...";
	case interface_unload_failed:
		return "unload returns failure";
	case dll_file_load_failed:
		return "dll file loading failure";
	case skip_unload_call_because_load_failed:
		return "Skip the unload call as load failed";
	case skip_unload_call_because_unload_not_found:
		return "Skip the unload call as it was not found";
	default:
		return "Something fucked up.";
	}
}

std::string_view to_ansi_colored_string(Cshiori::Error err) {
	using enum Cshiori::Error;
	switch(err) {
	case interface_load_not_found:
		return SET_RED "Necessary interface \"" SET_GREEN "load" SET_RED "\" not found" RESET_COLOR;
	case interface_unload_not_found:
		return SET_RED "Necessary interface \"" SET_GREEN "unload" SET_RED "\" not found" RESET_COLOR;
	case interface_request_not_found:
		return SET_RED "Necessary interface \"" SET_GREEN "request" SET_RED "\" not found" RESET_COLOR;
	case interface_load_failed:
		return SET_RED "Failed to " SET_GREEN "load" SET_RED ", unloading file..." RESET_COLOR;
	case interface_unload_failed:
		return SET_GREEN "unload" SET_RED " returns failure" RESET_COLOR;
	case dll_file_load_failed:
		return SET_RED "dll file loading failure" RESET_COLOR;
	case skip_unload_call_because_load_failed:
		return SET_RED "Skip the " SET_GREEN "unload" SET_RED " call as " SET_GREEN "load" SET_RED " failed" RESET_COLOR;
	case skip_unload_call_because_unload_not_found:
		return SET_RED "Skip the " SET_GREEN "unload" SET_RED " call as it was not found" RESET_COLOR;
	default:
		return SET_RED "Something fucked up." RESET_COLOR;
	}
}


std::string_view to_string(Cshiori::Warning warn) {
	using enum Cshiori::Warning;
	switch (warn) {
	case interface_CI_check_not_found:
		return "Interface \"CI_check_failed\" not found";
	case interface_logsend_not_found:
		return "Interface \"logsend\" not found";
	case logsend_failed:
		return "failed when set logsend hwnd";
	default:
		return "Something fucked up.";
	}
}
std::string_view to_ansi_colored_string(Cshiori::Warning warn) {
	using enum Cshiori::Warning;
	switch (warn) {
	case interface_CI_check_not_found:
		return SET_YELLOW "Interface \"" SET_GREEN "CI_check_failed" SET_YELLOW "\" not found" RESET_COLOR;
	case interface_logsend_not_found:
		return SET_YELLOW "Interface \"" SET_GREEN "logsend" SET_YELLOW "\" not found" RESET_COLOR;
	case logsend_failed:
		return SET_YELLOW "failed when set logsend hwnd" RESET_COLOR;
	default:
		return SET_YELLOW "Something fucked up." RESET_COLOR;
	}
}

#pragma once
#include <Windows.h>
#include "string2HGLOBAL.hpp"
#include "../file/GetFilename_sPath.hpp"
#include "../codepage.hpp"
enum class CshioriError {
	interface_load_not_found,
	interface_unload_not_found,
	interface_request_not_found,

	interface_load_failed,
	interface_unload_failed,

	dll_file_load_failed,

	skip_unload_call_because_load_failed,
	skip_unload_call_because_interface_unload_not_found,
};
std::string_view to_string(CshioriError err);
std::string_view to_ansi_colored_string(CshioriError err);
class Cshiori {
	HINSTANCE dll = NULL;

	typedef BOOL __cdecl load_type_t(HGLOBAL h, long len);
	typedef load_type_t* load_type;
	typedef BOOL __cdecl unload_type_t();
	typedef unload_type_t* unload_type;
	typedef HGLOBAL __cdecl request_type_t(HGLOBAL h, long* len);
	typedef request_type_t* request_type;

	//for yaya's CI
	typedef BOOL __cdecl CI_check_t(void);
	typedef CI_check_t* CI_check_type;
	typedef void __cdecl Set_loghandler_t(void (*loghandler)(const wchar_t *str, int mode, int id));
	typedef Set_loghandler_t* Set_loghandler_type;
	typedef bool __cdecl logsender_t(long hwnd);
	typedef logsender_t* logsender_type;

	//error_logger
	typedef void __cdecl error_logger_t(CshioriError);
	typedef error_logger_t* error_logger_type;

	std::wstring filename;
	load_type load=NULL;
	unload_type unload=NULL;
	request_type request=NULL;
	CI_check_type checker=NULL;
	logsender_type logsender=NULL;
	HWND hwnd_for_logsender=NULL;
	void (*loghandler)(const wchar_t *str, int mode, int id)=NULL;
	CODEPAGE_n::CODEPAGE cp=CODEPAGE_n::CP_UTF8;
	bool loadok=1,set_logsend_ok=1;
	error_logger_type error_logger=NULL;

	void call_error_logger(CshioriError err) {
		if (error_logger)
			error_logger(err);
	}

	void init_methods();
	void call_load(LPCWSTR pszFileName);
	bool call_unload();
	bool set_logsend(HWND hwnd);
public:
	bool All_OK();
	Cshiori(error_logger_type error_logger_p = NULL);
	void SetTo(LPCWSTR pszFileName);
	Cshiori(LPCWSTR pszFileName, error_logger_type error_logger_p = NULL);
	~Cshiori();
	void Doreload();
	bool Dounload();
	void SetCodePage(std::wstring);
	void SetCodePage(CODEPAGE_n::CODEPAGE);
	std::string operator()(std::string);
	std::wstring operator()(std::wstring);
	bool CI_check_failed();
	bool		 can_make_CI_check();
	bool		 can_set_logsend();
	void Set_loghandler(void (*loghandler)(const wchar_t *str, int mode, int id));
	void set_logsend_hwnd(HWND hwnd);
	bool is_logsend_ok();
};

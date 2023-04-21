//copy from https://github.com/ponapalt/csaori
//
#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <Windows.h>

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:26812)//enum class安利警告diss
#endif
namespace CODEPAGE_n{
	#undef CP_UTF8
	#undef CP_ACP

	typedef enum {
		CP_SJIS=932,
		CP_EUCJP=20932,
		CP_ISO2022JP=50220,
		CP_UTF8=65001,
		CP_ACP=0
	} CODEPAGE;

	//no init lambda for c++23's string::resize_and_overwrite
	inline auto no_init_buf = [](auto ptr, auto count) {return count;};
	
	[[nodiscard]]inline std::string UnicodeToMultiByte(const wchar_t *Source, unsigned int CodePage, DWORD Flags=0)
	{
		if ( Source && *Source ) {
			if (int Len = ::WideCharToMultiByte(CodePage, Flags, Source, wcslen(Source), NULL, 0, NULL, NULL)) {
				std::string str;
				str.resize_and_overwrite(Len, no_init_buf);
				if(Len = ::WideCharToMultiByte(CodePage, Flags, Source, wcslen(Source), str.data(), static_cast<int>(str.size()), NULL, NULL)) {
					return str;
				}
			}
		}
		return {};
	}

	[[nodiscard]]inline std::wstring MultiByteToUnicode(const char* Source, unsigned int CodePage, DWORD Flags=0)
	{
		if ( Source && *Source ) {
			if (int Len = ::MultiByteToWideChar(CodePage, Flags, Source, strlen(Source), NULL, 0)) {
				std::wstring str;
				str.resize_and_overwrite(Len, no_init_buf);
				if(Len = ::MultiByteToWideChar(CodePage, Flags, Source, strlen(Source), str.data(), static_cast<int>(str.size()))) {
					return str;
				}
			}
		}
		return {};
	}

	[[nodiscard]]inline std::string UnicodeToMultiByte(const std::wstring&Source, unsigned int CodePage, DWORD Flags=0)
	{
		if ( Source.size() ) {
			if (int Len = ::WideCharToMultiByte(CodePage, Flags, Source.c_str(), Source.size(), NULL, 0, NULL, NULL)) {
				std::string str;
				str.resize_and_overwrite(Len, no_init_buf);
				if(Len = ::WideCharToMultiByte(CodePage, Flags, Source.c_str(), Source.size(), str.data(), static_cast<int>(str.size()), NULL, NULL)) {
					return str;
				}
			}
		}
		return {};
	}

	[[nodiscard]]inline std::wstring MultiByteToUnicode(const std::string&Source, unsigned int CodePage, DWORD Flags=0)
	{
		if ( Source.size() ) {
			if (int Len = ::MultiByteToWideChar(CodePage, Flags, Source.c_str(), Source.size(), NULL, 0)) {
				std::wstring str;
				str.resize_and_overwrite(Len, no_init_buf);
				if(Len = ::MultiByteToWideChar(CodePage, Flags, Source.c_str(), Source.size(), str.data(), static_cast<int>(str.size()))) {
					return str;
				}
			}
		}
		return {};
	}

	[[nodiscard]]inline std::string UnicodeToMultiByte(const std::wstring_view &Source, unsigned int CodePage, DWORD Flags = 0) {
		if(Source.size()) {
			if(int Len = ::WideCharToMultiByte(CodePage, Flags, Source.data(), Source.size(), NULL, 0, NULL, NULL)) {
				std::string str;
				str.resize_and_overwrite(Len, no_init_buf);
				if(Len = ::WideCharToMultiByte(CodePage, Flags, Source.data(), Source.size(), str.data(), static_cast<int>(str.size()), NULL, NULL)) {
					return str;
				}
			}
		}
		return {};
	}

	[[nodiscard]]inline std::wstring MultiByteToUnicode(const std::string_view &Source, unsigned int CodePage, DWORD Flags = 0) {
		if(Source.size()) {
			if(int Len = ::MultiByteToWideChar(CodePage, Flags, Source.data(), Source.size(), NULL, 0)) {
				std::wstring str;
				str.resize_and_overwrite(Len, no_init_buf);
				if(Len = ::MultiByteToWideChar(CodePage, Flags, Source.data(), Source.size(), str.data(), static_cast<int>(str.size()))) {
					return str;
				}
			}
		}
		return {};
	}
	
	[[nodiscard]]inline std::wstring_view CodePagetoString(unsigned int cset){
		switch(cset){
			case CP_SJIS:
				return L"Shift_JIS";
			case CP_ISO2022JP:
				return L"ISO-2022-JP";
			case CP_EUCJP:
				return L"EUC-JP";
			case CP_UTF8:
				return L"UTF-8";
		}
		return L"unknown charset";
	}

	[[nodiscard]]inline int _strnicmp(const char *s1, std::string_view s2, size_t n)
	{
		if(s2.size() < n) {
			n = s2.size();
			const int ret = ::_strnicmp(s1, s2.data(), n);
			if(ret == 0) {
				return -1;
			}
			return ret;
		}
		if(s2.size() > n) {
			const int ret = ::_strnicmp(s1, s2.data(), n);
			if(ret == 0) {
				return 1;
			}
			return ret;
		}
		return ::_strnicmp(s1, s2.data(), n);
	}
	[[nodiscard]]inline int _strnicmp(std::string_view s1,const char *s2, size_t n){
		const auto aret=_strnicmp(s2,s1,n);
		if(aret==0){
			return 0;
		}
		return -aret;
	}
	//_wcsnicmp
	[[nodiscard]]inline int _wcsnicmp(const wchar_t *s1, std::wstring_view s2, size_t n)
	{
		if(s2.size() < n) {
			n = s2.size();
			const int ret = ::_wcsnicmp(s1, s2.data(), n);
			if(ret == 0) {
				return -1;
			}
			return ret;
		}
		if(s2.size() > n) {
			const int ret = ::_wcsnicmp(s1, s2.data(), n);
			if(ret == 0) {
				return 1;
			}
			return ret;
		}
		return ::_wcsnicmp(s1, s2.data(), n);
	}
	[[nodiscard]]inline int _wcsnicmp(std::wstring_view s1,const wchar_t *s2, size_t n){
		const auto aret=_wcsnicmp(s2,s1,n);
		if(aret==0){
			return 0;
		}
		return -aret;
	}

	[[nodiscard]]inline CODEPAGE StringtoCodePage(std::string_view str)
	{
		if ( str.size() ) {
			if ( _strnicmp(str,"shift_jis",9) == 0 ) {
				return CP_SJIS;
			}
			if ( _strnicmp(str,"x-sjis",6) == 0 ) {
				return CP_SJIS;
			}
			if ( _strnicmp(str,"iso-2022-jp",11) == 0 ) {
				return CP_ISO2022JP;
			}
			if ( _strnicmp(str,"euc-jp",6) == 0 ) {
				return CP_EUCJP;
			}
			if ( _strnicmp(str,"x-euc-jp",8) == 0 ) {
				return CP_EUCJP;
			}
			if ( _strnicmp(str,"utf-8",5) == 0 ) {
				return CP_UTF8;
			}
		}
		return CP_SJIS;
	}

	[[nodiscard]]inline CODEPAGE StringtoCodePage(std::wstring_view str)
	{
		if ( str.size() ) {
			if ( _wcsnicmp(str,L"shift_jis",9) == 0 ) {
				return CP_SJIS;
			}
			if ( _wcsnicmp(str,L"x-sjis",6) == 0 ) {
				return CP_SJIS;
			}
			if ( _wcsnicmp(str,L"iso-2022-jp",11) == 0 ) {
				return CP_ISO2022JP;
			}
			if ( _wcsnicmp(str,L"euc-jp",6) == 0 ) {
				return CP_EUCJP;
			}
			if ( _wcsnicmp(str,L"x-euc-jp",8) == 0 ) {
				return CP_EUCJP;
			}
			if ( _wcsnicmp(str,L"utf-8",5) == 0 ) {
				return CP_UTF8;
			}
		}
		return CP_SJIS;
	}
}		// namespace CODEPAGE_n
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

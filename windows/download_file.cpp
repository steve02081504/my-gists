#pragma once
#include <string>
#include <stdexcept>
#include <windows.h>
#include <wininet.h>
#include <Wincrypt.h>//md5
#include "get_temp_path.cpp"
#include "download_file.hpp"
#include "../codepage.hpp"
#pragma comment(lib, "WinInet.lib")
#pragma comment(lib, "Crypt32.lib")
std::wstring get_domain_form_url(const std::wstring& url) {
	std::wstring domain;
	auto		 domain_begin = url.find(L"://");
	if(domain_begin != std::wstring::npos)
		domain_begin += 3;
	else
		domain_begin = url.find(L':');
	if(domain_begin != std::wstring::npos) {
		auto domain_end = url.find_first_of(L"/\\", domain_begin);
		if(domain_end == std::wstring::npos)
			domain_end = url.length();
		domain = url.substr(domain_begin, domain_end - domain_begin);
	}
	return domain;
}
std::wstring download_file(const wchar_t* url, const wchar_t* file) {
	//open file as hFileHandle, if file already exists, append of file
	HANDLE hFileHandle = CreateFileW(file, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFileHandle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("CreateFileW failed");
	}
	//get the file size
	DWORD dwFileSize = GetFileSize(hFileHandle, NULL);
	if(dwFileSize)
		SetFilePointer(hFileHandle, 0, NULL, FILE_END);
	//download file
	HINTERNET hInternet = InternetOpenW(NetAgentName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hInternet == NULL) {
		throw std::runtime_error("InternetOpenW failed");
	}
	//get file size and md5
	DWORD	  dwNetFileSize = 0;
	std::string NetFileMd5{};
	NetFileMd5.resize_and_overwrite(25, CODEPAGE_n::no_init_buf);
	HINTERNET hFile	 = InternetOpenUrlW(hInternet, url, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	{
		LPVOID lpBuffer		  = &dwNetFileSize;
		DWORD  dwBufferLength = sizeof(dwNetFileSize);
		if(!HttpQueryInfoW(hFile, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, lpBuffer, &dwBufferLength, NULL)) {
			dwNetFileSize = DWORD(-1);
			//re downlod file
			CloseHandle(hFileHandle);
			hFileHandle = CreateFileW(file, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			dwFileSize	= 0;
			#if defined(_DEBUG)
				//GetLastError as string
				DWORD dwError = GetLastError();
				LPWSTR lpMsgBuf=NULL;
				size_t size		= FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
												NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpMsgBuf, 0, NULL);
				std::wstring message(lpMsgBuf, size);
				LocalFree(lpMsgBuf);
			#endif
		}
		dwBufferLength = NetFileMd5.size();
		if(!HttpQueryInfoA(hFile, HTTP_QUERY_CONTENT_MD5, (LPVOID)NetFileMd5.data(), (LPDWORD)&dwBufferLength, NULL)) {
			NetFileMd5.clear();
		}
	}
	if(hFile == NULL) {
		InternetCloseHandle(hInternet);
		auto domain = get_domain_form_url(url);
		if(domain.empty())
			throw std::runtime_error("InternetOpenUrlW failed");
		else
			throw std::runtime_error("InternetOpenUrlW failed, Please ensure that you can link to " + CODEPAGE_n::UnicodeToMultiByte(domain, 0));
	}
	bool is_re_download = 0;
	auto reinit_for_redownload = [&]() {
		//re downlod file
		CloseHandle(hFileHandle);
		hFileHandle = CreateFileW(file, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFileHandle == INVALID_HANDLE_VALUE) {
			InternetCloseHandle(hFile);
			InternetCloseHandle(hInternet);
			throw std::runtime_error("CreateFileW failed");
		}
		auto setresult = InternetSetFilePointer(hFile, 0, NULL, FILE_BEGIN, 0);
		if(setresult == INVALID_SET_FILE_POINTER) {
			//re open net file
			InternetCloseHandle(hFile);
			hFile = InternetOpenUrlW(hInternet, url, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
		}
	};
	std::string FileMd5{};
	auto re_cal_file_md5 = [&]() {
		HCRYPTPROV hFileProv = 0;
		HCRYPTHASH hFileHash = 0;
		if(!CryptAcquireContextW(&hFileProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
			throw std::runtime_error("CryptAcquireContextW failed");
		}
		if(!CryptCreateHash(hFileProv, CALG_MD5, 0, 0, &hFileHash)) {
			CryptReleaseContext(hFileProv, 0);
			throw std::runtime_error("CryptCreateHash failed");
		}
		SetFilePointer(hFileHandle, 0, NULL, FILE_BEGIN);
		DWORD dwReadSize = 0;
		char	 buf[1024];
		while(ReadFile(hFileHandle, buf, sizeof(buf), &dwReadSize, NULL) && dwReadSize) {
			if(!CryptHashData(hFileHash, (BYTE*)buf, dwReadSize, 0)) {
				CryptDestroyHash(hFileHash);
				CryptReleaseContext(hFileProv, 0);
				throw std::runtime_error("CryptHashData failed");
			}
		}
		DWORD dwHashSize = 16;
		BYTE	 hash[16];
		if(!CryptGetHashParam(hFileHash, HP_HASHVAL, hash, &dwHashSize, 0)) {
			CryptDestroyHash(hFileHash);
			CryptReleaseContext(hFileProv, 0);
			throw std::runtime_error("CryptGetHashParam failed");
		}
		CryptDestroyHash(hFileHash);
		CryptReleaseContext(hFileProv, 0);
		FileMd5.resize_and_overwrite(25, CODEPAGE_n::no_init_buf);
		DWORD dwHashLen = FileMd5.size();
		//base64
		if(!CryptBinaryToStringA(hash, dwHashSize, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, (LPSTR)FileMd5.data(), (DWORD*)&dwHashLen))
			FileMd5.clear();
	};
	auto is_md5_match = [&]() {
		if(NetFileMd5.empty())
			return true;
		if(FileMd5.empty()) {
			re_cal_file_md5();
		}
		return (FileMd5 == NetFileMd5);
	};
	if(dwFileSize == dwNetFileSize)		  //if file size is equal, no need to download
	{
		if(!is_md5_match()) {
			reinit_for_redownload();
		}
		else {
			InternetCloseHandle(hFile);
			InternetCloseHandle(hInternet);
			CloseHandle(hFileHandle);
			return file;
		}
	}
	DWORD		  dwBytesRead	 = 0;
	DWORD		  dwBytesWritten = 0;
	unsigned char buffer[1024];
	bool		  full_download = false;
	//Skip downloaded sections if dwFileSize is not 0
	if(dwFileSize != 0) {
		auto setresult = InternetSetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN, 0);
		if(setresult == INVALID_SET_FILE_POINTER) {
			reinit_for_redownload();
		}
	}
download_loop:
	while(InternetReadFile(hFile, buffer, 1024, &dwBytesRead)) {
		if(dwBytesRead > 0)
			WriteFile(hFileHandle, buffer, dwBytesRead, &dwBytesWritten, NULL);
		else {
			full_download = true;
			break;
		}
	}
	//match md5
	if(full_download) {
		re_cal_file_md5();
		if(!is_md5_match()) {
			if(is_re_download) {
				InternetCloseHandle(hFile);
				InternetCloseHandle(hInternet);
				CloseHandle(hFileHandle);
				throw std::runtime_error("InternetReadFile failed");
			}
			else {
				reinit_for_redownload();
				is_re_download = true;
				goto download_loop;
			}
		}
	}
	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);
	CloseHandle(hFileHandle);
	if(!full_download) {
		if(dwNetFileSize == DWORD(-1))
			DeleteFileW(file);
		throw std::runtime_error("InternetReadFile failed");
	}
	return file;
}
std::wstring download_file(const std::wstring& url, const wchar_t* file) {
	return download_file(url.c_str(), file);
}
std::wstring download_file(const wchar_t* url, const std::wstring& file) {
	return download_file(url, file.c_str());
}
std::wstring download_file(const std::wstring& url, const std::wstring& file) {
	return download_file(url.c_str(), file.c_str());
}

std::wstring download_temp_file(const std::wstring& url, const std::wstring& file_suffix) {
	return download_file(url, get_temp_filename(file_suffix));
}
std::wstring download_temp_file(const wchar_t* url, const std::wstring& file_suffix) {
	return download_file(url, get_temp_filename(file_suffix));
}
std::wstring download_temp_file(const std::wstring& url, const wchar_t* file_suffix) {
	return download_file(url, get_temp_filename(file_suffix));
}
std::wstring download_temp_file(const wchar_t* url, const wchar_t* file_suffix) {
	return download_file(url, get_temp_filename(file_suffix));
}
std::wstring download_temp_file(const std::wstring& url) {
	return download_file(url, get_temp_filename());
}
std::wstring download_temp_file(const wchar_t* url) {
	return download_file(url, get_temp_filename());
}

std::wstring download_file_to_temp_dir(const std::wstring& url, const std::wstring& filename) {
	auto filepath = get_temp_path() + filename;
	return download_file(url, filepath);
}
std::wstring download_file_to_temp_dir(const wchar_t* url, const std::wstring& filename) {
	auto filepath = get_temp_path() + filename;
	return download_file(url, filepath);
}
std::wstring download_file_to_temp_dir(const std::wstring& url, const wchar_t* filename) {
	auto filepath = std::wstring{} + get_temp_path() + filename;
	return download_file(url, filepath);
}
std::wstring download_file_to_temp_dir(const wchar_t* url, const wchar_t* filename) {
	auto filepath = std::wstring{} + get_temp_path() + filename;
	return download_file(url, filepath);
}

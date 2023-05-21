#pragma once
#define NOMINMAX
#include <windows.h>

#include <string>
#include <algorithm>

class flush_t{};
inline constexpr flush_t flush{};
class endline_t{};
inline constexpr endline_t endline{};
template<class char_T>
struct X_times_output_t{
	size_t _times;
	char_T _ch;
};
template<class char_T>
inline X_times_output_t<char_T> X_times(const size_t times, const char_T ch)noexcept{
	return X_times_output_t<char_T>{times, ch};
}

class base_out_t{
	HANDLE _h;
public:
	base_out_t(HANDLE h)noexcept : _h(h) {}

	base_out_t& operator<<(const std::wstring& str) noexcept {
		DWORD written;
		WriteConsoleW(_h, str.c_str(), str.size(), &written, nullptr);
		return *this;
	}
	base_out_t& operator<<(const std::string& str) noexcept {
		DWORD written;
		WriteConsoleA(_h, str.c_str(), str.size(), &written, nullptr);
		return *this;
	}
	base_out_t& operator<<(const std::wstring_view& str) noexcept {
		DWORD written;
		WriteConsoleW(_h, str.data(), str.size(), &written, nullptr);
		return *this;
	}
	base_out_t& operator<<(const std::string_view& str) noexcept {
		DWORD written;
		WriteConsoleA(_h, str.data(), str.size(), &written, nullptr);
		return *this;
	}
	static constexpr size_t tmp_buf_size = 512;
	base_out_t& operator<<(const X_times_output_t<wchar_t>& x) noexcept {
		wchar_t buf[tmp_buf_size];
		std::fill_n(buf, std::min(x._times, tmp_buf_size), x._ch);
		DWORD written;
		size_t times = x._times;
		while(times > 0){
			WriteConsoleW(_h, buf, std::min(times, tmp_buf_size), &written, nullptr);
			times -= written;
		}
		return *this;
	}
	base_out_t& operator<<(const X_times_output_t<char>& x) noexcept {
		char buf[tmp_buf_size];
		std::fill_n(buf, std::min(x._times, tmp_buf_size), x._ch);
		DWORD written;
		size_t times = x._times;
		while(times > 0){
			WriteConsoleA(_h, buf, std::min(times, tmp_buf_size), &written, nullptr);
			times -= written;
		}
		return *this;
	}
	base_out_t& operator<<(const wchar_t* str) noexcept {
		const std::wstring_view str_view(str);
		return operator<<(str_view);
	}
	base_out_t& operator<<(const char* str) noexcept {
		const std::string_view str_view(str);
		return operator<<(str_view);
	}
	base_out_t& operator<<(const wchar_t ch) noexcept {
		DWORD written;
		WriteConsoleW(_h, &ch, 1, &written, nullptr);
		return *this;
	}
	base_out_t& operator<<(const char ch) noexcept {
		DWORD written;
		WriteConsoleA(_h, &ch, 1, &written, nullptr);
		return *this;
	}
	base_out_t& operator<<(const flush_t&) noexcept {
		FlushFileBuffers(_h);
		return *this;
	}
	base_out_t& operator<<(const endline_t&) noexcept {
		DWORD written;
		WriteConsoleW(_h, L"\n", 1, &written, nullptr);
		return operator<<(flush);
	}
};

class out_t : public base_out_t{
public:
	out_t()noexcept : base_out_t(GetStdHandle(STD_OUTPUT_HANDLE)) {}
};
inline out_t out;

class err_t : public base_out_t{
public:
	err_t()noexcept : base_out_t(GetStdHandle(STD_ERROR_HANDLE)) {}
};
inline err_t err;

class nullstream_t: public base_out_t{
public:
	nullstream_t()noexcept : base_out_t(INVALID_HANDLE_VALUE) {}
};
inline nullstream_t nullstream;

class base_in_t{
	HANDLE _h;
	bool _is_eof=0;
public:
	base_in_t(HANDLE h)noexcept : _h(h) {}

	base_in_t& operator>>(wchar_t& ch) noexcept {
		DWORD read;
		ReadConsoleW(_h, &ch, 1, &read, nullptr);
		if(ch==L'\x1a') _is_eof=1;
		return *this;
	}
	base_in_t& operator>>(char& ch) noexcept {
		DWORD read;
		ReadConsoleA(_h, &ch, 1, &read, nullptr);
		if(ch=='\x1a') _is_eof=1;
		return *this;
	}
	friend base_in_t& getline(base_in_t& in, std::wstring& str) noexcept {
		str.clear();
		wchar_t ch;
		while(in >> ch){
			if(ch == L'\n' || ch==L'\x1a') break;
			str.push_back(ch);
		}
		str.size() && str.back() == L'\r' ? str.pop_back() : void();
		return in;
	}
	friend base_in_t& getline(base_in_t& in, std::string& str) noexcept {
		str.clear();
		char ch;
		while(in >> ch){
			if(ch == '\n' || ch=='\x1a') break;
			str.push_back(ch);
		}
		str.size() && str.back() == '\r' ? str.pop_back() : void();
		return in;
	}
	[[nodiscard]] explicit operator bool() const noexcept {
		if(is_null()) return false;
		if(_is_eof) return false;
		if(is_file()){
			//check if the file is at the end
			LARGE_INTEGER pos;
			GetFileSizeEx(_h, &pos);
			return pos.QuadPart != SetFilePointerEx(_h, {0, 0}, &pos, FILE_CURRENT);
		}
		return true;
	}
	[[nodiscard]] bool eof() const noexcept {
		return !operator bool();
	}
	[[nodiscard]] bool is_file() const noexcept {
		return GetFileType(_h) == FILE_TYPE_DISK;
	}
	[[nodiscard]] bool is_console() const noexcept {
		return GetFileType(_h) == FILE_TYPE_CHAR;
	}
	[[nodiscard]] bool is_pipe() const noexcept {
		return GetFileType(_h) == FILE_TYPE_PIPE;
	}
	[[nodiscard]] bool is_null() const noexcept {
		return _h == INVALID_HANDLE_VALUE || !_h;
	}
};

class in_t : public base_in_t{
public:
	in_t()noexcept : base_in_t(GetStdHandle(STD_INPUT_HANDLE)) {}
};
inline in_t in;

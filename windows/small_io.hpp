#include <windows.h>
#include <string>

class flush_t{};
inline constexpr flush_t flush{};
class endline_t{};
inline constexpr endline_t endline{};

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

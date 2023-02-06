//const char
[[nodiscard]] bool can_read_file(const char* filename);

[[nodiscard]] bool can_write_file(const char* filename);

[[nodiscard]] bool can_read_write_file(const char* filename);
//wconst char
[[nodiscard]] bool can_read_file(const wchar_t* filename);

[[nodiscard]] bool can_write_file(const wchar_t* filename);

[[nodiscard]] bool can_read_write_file(const wchar_t* filename);

//std::string
#include <string>
[[nodiscard]] inline bool can_read_file(const std::string& filename) {
	return can_read_file(filename.c_str());
}

[[nodiscard]] inline bool can_write_file(const std::string& filename) {
	return can_write_file(filename.c_str());
}

[[nodiscard]] inline bool can_read_write_file(const std::string& filename) {
	return can_read_write_file(filename.c_str());
}
//std::wstring
[[nodiscard]] inline bool can_read_file(const std::wstring& filename) {
	return can_read_file(filename.c_str());
}

[[nodiscard]] inline bool can_write_file(const std::wstring& filename) {
	return can_write_file(filename.c_str());
}

[[nodiscard]] inline bool can_read_write_file(const std::wstring& filename) {
	return can_read_write_file(filename.c_str());
}

//can_t ver
template<class T>
[[nodiscard]] inline bool can_t_read_file(T&& filename) {
	return !can_read_file(std::forward<T>(filename));
}

template<class T>
[[nodiscard]] inline bool can_t_write_file(T&& filename) {
	return !can_write_file(std::forward<T>(filename));
}

template<class T>
[[nodiscard]] inline bool can_t_read_write_file(T&& filename) {
	return !can_read_write_file(std::forward<T>(filename));
}

#include <string>

bool Split(const std::wstring& str, std::wstring& s0, std::wstring& s1, std::wstring_view sepstr);
bool Split(const std::string& str, std::string& s0, std::string& s1, std::string_view sepstr);
//string_view
bool Split(const std::wstring_view& str, std::wstring_view& s0, std::wstring_view& s1, std::wstring_view sepstr);
bool Split(const std::string_view& str, std::string_view& s0, std::string_view& s1, std::string_view sepstr);
//string & string_view
bool Split(const std::wstring& str, std::wstring_view& s0, std::wstring_view& s1, std::wstring_view sepstr);
bool Split(const std::string& str, std::string_view& s0, std::string_view& s1, std::string_view sepstr);

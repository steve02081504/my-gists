#include <string>
#include <string_view>
template<class char_t>
inline std::basic_string<char_t>&replace_all(std::basic_string<char_t>&a,const std::basic_string_view<char_t> b,const std::basic_string_view<char_t> c) {
	auto i = a.find(b);
	while (i!=std::basic_string<char_t>::npos) {
		a.replace(i, b.size(), c);
		i = a.find(b,i+c.size());
	}
	return a;
};

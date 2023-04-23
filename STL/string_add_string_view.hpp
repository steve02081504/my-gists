#pragma once
#include <string>
#include <string_view>

template <typename charT, typename traits, typename Allocator>
inline std::basic_string<charT, traits, Allocator> operator+(
	std::basic_string<charT, traits, Allocator> s0,
	std::basic_string_view<charT, traits> s1) {
	return s0.append(s1);
}

template <typename charT, typename traits, typename Allocator>
inline std::basic_string<charT, traits, Allocator> operator+(
	std::basic_string_view<charT, traits> s0,
	std::basic_string<charT, traits, Allocator> s1) {
	return s1.insert(0, s0);
}

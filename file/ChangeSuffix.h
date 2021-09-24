template<class char_t>
inline auto ChangeSuffix(std::basic_string<char_t> name,std::basic_string_view<char_t> newSuffix){
	auto point=name.rfind(L".",std::max(name.rfind(L"\\"),name.rfind(L"/")));
	if(point!=std::basic_string<char_t>::npos)
		name.erase(point);
	return name+L"."+newSuffix.data();
}

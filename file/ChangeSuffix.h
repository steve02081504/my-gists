inline auto ChangeSuffix(string_t name,string_t newSuffix){
	auto point=name.rfind(L".",std::max(name.rfind(L"\\"),name.rfind(L"/")));
	if(point!=string_t::npos)
		name.erase(point);
	return name+L"."+newSuffix;
}

template<class char_t>
inline bool fgetstring(std::basic_string<char_t>&aret,FILE*from){
	int c;
	aret="";
	while((c=fgetc(from))!=EOF && c!='\n')
		aret+=char(c);
	return c!=EOF;
}

template<class char_t>
inline bool fgetstring(std::basic_string<char_t>&aret,FILE*from){
	int c;
	aret="";
	while((c=fgetc(from))!=EOF && c!='\n')
		if(c=='\r'){
			c=fgetc(from);
			if(!(c==EOF||c=='\n'))
				ungetc(c,from);
			break;
		}
		else
			aret+=char_t(c);
	return !(c==EOF&&aret.empty());
}

template<class char_t>
inline bool fgetstring(std::basic_string<char_t>&aret,FILE*from){
	int c;
	aret="";
	bool success=0;
	while(1){
		c=fgetc(from);
		switch(c){
		case '\r':
			c=fgetc(from);
			if(!(c==EOF||c=='\n'))
				ungetc(c,from);
			[[fallthrough]];
		case '\n':
			success=1;
			[[fallthrough]];
		case EOF:
			goto tret;
		default:
			success=1;
			aret+=char_t(c);
			break;
		}
	}
tret:
	return success;
}

inline void fputs(const std::string& str,FILE*to){
	for(auto c:str)
		fputc(c,to);
}

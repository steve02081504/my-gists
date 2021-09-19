inline void fputs(const string& str,FILE*to){
	string aret;
	for(auto c:str)
		fputc(c,to);
	fputc('\n',to);
}

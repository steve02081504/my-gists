inline bool fgetstring(string&aret,FILE*from){
	int c;
	aret="";
	while((c=fgetc(from))!=EOF && c!='\n')
		aret+=char(c);
	return c!=EOF;
}

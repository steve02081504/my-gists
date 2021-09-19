struct Cshiori{
	HINSTANCE dll=NULL;

	typedef BOOL __cdecl load_type_t(HGLOBAL h, long len);
	typedef load_type_t* load_type;
	typedef BOOL __cdecl unload_type_t();
	typedef unload_type_t* unload_type;
	typedef HGLOBAL __cdecl request_type_t(HGLOBAL h, long* len);
	typedef request_type_t* request_type;

	load_type load=NULL;
	bool loadok=1;
	unload_type unload=NULL;
	request_type request=NULL;

	bool All_OK();

	Cshiori();
	void SetTo(LPCWSTR pszFileName);
	Cshiori(LPCWSTR pszFileName);
	~Cshiori();
	
};

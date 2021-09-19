struct Cshiori{
	HINSTANCE dll=NULL;

	typedef BOOL __cdecl load_type_t(HGLOBAL h, long len);
	typedef load_type_t* load_type;
	typedef BOOL __cdecl unload_type_t();
	typedef unload_type_t* unload_type;
	typedef HGLOBAL __cdecl request_type_t(HGLOBAL h, long* len);
	typedef request_type_t* request_type;

	load_type load=NULL;
	unload_type unload=NULL;
	request_type request=NULL;

	void init_methods(){
		load=(load_type)GetProcAddress(dll,"load");
		unload=(unload_type)GetProcAddress(dll,"unload");
		request=(request_type)GetProcAddress(dll,"request");
	}
	Cshiori(){}
	void SetTo(LPCWSTR pszFileName){
		dll=LoadLibrary(pszFileName);
		init_methods();
	}
	Cshiori(LPCWSTR pszFileName){
		SetTo(pszFileName);
	}
	~Cshiori(){
		FreeLibrary(dll);
	}
};

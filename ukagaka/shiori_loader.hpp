struct Cshiori{
	HINSTANCE dll=NULL;

	typedef BOOL __cdecl (*load_type)(HGLOBAL h, long len);
	typedef BOOL __cdecl (*unload_type)();
	typedef HGLOBAL __cdecl (*request_type)(HGLOBAL h, long *len)

	load_type load=NULL;
	unload_type unload=NULL;
	request_type request=NULL;

	init_methods(){
		load=(load_type)GetProcAddress(dll,L"load");
		unload=(unload_type)GetProcAddress(dll,L"unload");
		request=(request_type)GetProcAddress(dll,L"request");
	}
	Cshiori(LPCWSTR pszFileName){
		dll=LoadLibrary(pszFileName);
		init_methods();
	}
	~Cshiori(){
		FreeLibrary(dll);
	}
};

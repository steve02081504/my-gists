#pragma once
#ifndef DONT_USE_SOCKET
#include "../socket.hpp"
#endif		 // !DONT_USE_SOCKET
#include "../codepage.hpp"
#include <map>
#include <time.h>

namespace SSTP_link_n{
	using namespace CODEPAGE_n;
	#ifndef DONT_USE_SOCKET
	using namespace Socket_link_n;
	#endif		 // !DONT_USE_SOCKET
	struct SSTP_link_arg_parts_t{
		std::wstring _name;
		std::wstring _var;
	};
	struct SSTP_link_args_t{
		std::map<std::wstring,std::wstring>_m;
		SSTP_link_args_t(std::initializer_list<SSTP_link_arg_parts_t>a){
			for (auto&x:a) {
				_m[x._name]=x._var;
			}
		}
		explicit SSTP_link_args_t(std::wstring a){
			if(a.find(L":") > a.find(L"\r\n"))
				a.erase(0,a.find(L"\r\n"));
			while(a!=L""){
				std::wstring t=a.substr(0,a.find(L"\r\n"));
				if (t.find(L": ")!=t.npos){
					_m[t.substr(0,t.find(L": "))]=t.substr(t.find(L": ")+2);
					a.erase(0,a.find(L"\r\n")+2);
					while (!a.substr(0,a.find(L"\r\n")).find(L": ")){
						_m[t.substr(0,t.find(L": "))]+=L"\r\n"+a.substr(0,a.find(L"\r\n"));
						a.erase(0,a.find(L"\r\n")+2);
					}
				}else{
					a.erase(0,a.find(L"\r\n")+2);
				}
			}
		}
		template<class T>
		explicit SSTP_link_args_t(T&&a):SSTP_link_args_t(std::wstring(a)){}
		explicit operator std::wstring(){
			std::wstring aret;
			for (auto&x:_m) {
				aret += x.first + L": " + x.second + L"\r\n";
			}
			return aret;
		}
		auto var(){return _m[L"Script"];}
		bool has(std::wstring a){return _m.count(L"X-SSTP-PassThru-"+a);}
		auto operator[](std::wstring a){return _m[L"X-SSTP-PassThru-"+a];}
		std::map<std::wstring,std::wstring>& get_str_map(){return _m;}
	};

	template<class T>
	auto operator+(T&&a,SSTP_link_args_t&b) {return std::wstring(a)+std::wstring(b);}

	struct SSTP_ret_t{
		std::wstring _m;
		SSTP_link_args_t __m;
		SSTP_ret_t():__m({}){}
		SSTP_ret_t(std::wstring a):_m(a),__m(_m){}
		operator std::wstring(){return _m;}
		explicit operator SSTP_link_args_t(){return __m;}
		auto to_str(){return _m;}
		auto to_map(){return operator SSTP_link_args_t();}
		auto operator[](std::wstring a){return __m[a];}
		auto get_head(){return _m.substr(_m.find(L"SSTP/"),_m.find(L"\r\n"));}
		auto get_code(){return std::wcstoll(_m.substr(_m.find(L"SSTP/")+9,_m.find(L"SSTP/")+12).c_str(),NULL,10);}
		auto var(){return __m.var();}
		bool has(std::wstring a){return __m.has(a);}
		auto& to_str_map(){return __m.get_str_map();}
	};

	template<class T>
	auto&&operator<<(T&&a,SSTP_ret_t&b) {return a<<b.to_str();}
	template<class T>
	auto operator+(T&&a,SSTP_ret_t&b) {return a+b.to_str();}

	#define dwDataOfDirectSSTP 9801

	#ifndef DONT_USE_SOCKET
	struct SSTP_link_Socket_t {
		Socket_link_t*	 pSocket = nullptr;
		SSTP_link_args_t _header{};

		~SSTP_link_Socket_t() {
			delete pSocket;
		}

		bool Socket_link(std::string addr = "127.0.0.1", unsigned int port = 9821) {
			delete pSocket;
			try{pSocket = new Socket_link_t(addr, port);}
			catch (...) { return 0; }
			return 1;
		}
		bool link_to_ghost(HWND ghost) {
			if(ghost){
				_header._m[L"ReceiverGhostHWnd"] = std::to_wstring((size_t)ghost);
			}
			else {
				_header._m.erase(L"ReceiverGhostHWnd");
			}
			return ghost;
		}
		bool link_to_ghost(std::wstring ghost) {
			if (!ghost.empty()) {
				_header._m[L"ReceiverGhostName"] = ghost;
			}
			else {
				_header._m.erase(L"ReceiverGhostName");
			}
			return !ghost.empty();
		}
		void base_send(std::string massage) {
			if(!pSocket)
				return;
			else
				pSocket->base_send(massage);
		}
		std::string base_get_ret() {
			if(!pSocket)
				return "";
			else
				return pSocket->base_get_ret();
		}
		std::wstring get_SSTP_head(std::wstring SSTP_type){
			return SSTP_type+L"\r\n"+_header;
		}
		void before_SSTP_send(){
			if(pSocket)
				pSocket->relink();//SSTP server can't process "keep-alive" style connection like HTTP: http://ssp.shillest.net/bts/view.php?id=170#c384
			else
				Socket_link();
		}
	};
	#endif
	struct SSTP_link_Direct_t {
		bool usingDirectSSTP=0;
		HWND toghost=0;
		HWND replay_to=0;
		std::string aret;

		void link_to_ghost(HWND ghost) {
			toghost = ghost;
		}
		void base_send(std::string massage) {
			if(!toghost)
				return;
			COPYDATASTRUCT cdt;
			cdt.cbData = massage.size();
			cdt.lpData = (void*)massage.c_str();
			cdt.dwData = dwDataOfDirectSSTP;
			SendMessage(toghost, WM_COPYDATA, (WPARAM)replay_to, (LPARAM)&cdt);
		}
		std::string base_get_ret() {
			if(!toghost || !replay_to)
				return "";
			MSG msg;
			time_t sec=time(0);
			while(1) {
				if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if(aret.size())
						return aret;
				}
				else if(time(0) > sec + 7)
					return "";
				else
					Sleep(250);
			}
		}
		std::wstring get_SSTP_head(std::wstring SSTP_type){
			return SSTP_type;
		}
		void before_SSTP_send(){
			aret.clear();
		}
		void DirectSSTPprocess(COPYDATASTRUCT*pcdt) {
			aret = std::string((char*)pcdt->lpData, (size_t)pcdt->cbData);
		}
		void setReplayHwnd(HWND hwnd) {
			replay_to = hwnd;
		}
	};
	template<typename base_link_t>
	struct SSTP_link_T:base_link_t{
		SSTP_link_args_t _header;

		SSTP_link_T(
					SSTP_link_args_t header={{L"Charset",L"UTF-8"},{L"Sender",L"void"}}
					):
		_header(header){}

		std::wstring get_SSTP_head(std::wstring SSTP_type){
			return base_link_t::get_SSTP_head(SSTP_type) + L"\r\n" + _header;
		}
		std::wstring base_SSTP_send(std::wstring head,SSTP_link_args_t args){
			base_link_t::before_SSTP_send();
			{
				auto send=get_SSTP_head(head)+args+L"\r\n";
				auto charset=send;
				charset=charset.substr(charset.find(L"\r\nCharset: ")+11);
				charset=charset.substr(0,charset.find(L"\r\n"));
				base_link_t::base_send(UnicodeToMultiByte(send, StringtoCodePage(charset.c_str())));
			}
			{
				auto temp = base_link_t::base_get_ret();
				if(temp.size()) {
					auto charset = temp;
					charset		 = charset.substr(charset.find("\r\nCharset: ") + 11);
					charset		 = charset.substr(0, charset.find("\r\n"));
					return MultiByteToUnicode(temp, StringtoCodePage(charset.c_str()));
				}
				else
					return L"";
			}
		}
		SSTP_ret_t NOTYFY(SSTP_link_args_t args){
			return base_SSTP_send(L"NOTIFY SSTP/1.1",args);
		}
		SSTP_ret_t SEND(SSTP_link_args_t args){
			return base_SSTP_send(L"SEND SSTP/1.4",args);
		}
		SSTP_ret_t EXECUTE(SSTP_link_args_t args){
			return base_SSTP_send(L"EXECUTE SSTP/1.3",args);
		}
		SSTP_ret_t COMMUNICATE(SSTP_link_args_t args){
			return base_SSTP_send(L"COMMUNICATE SSTP/1.1",args);
		}
		SSTP_ret_t GIVE(SSTP_link_args_t args){
			return base_SSTP_send(L"GIVE SSTP/1.1",args);
		}
		SSTP_ret_t INSTALL(SSTP_link_args_t args){
			return base_SSTP_send(L"INSTALL SSTP/1.0",args);
		}
		/*
		define this in aya:
		SHIORI_EV.On_Has_Event : void {
			SHIORI_FW.Make_X_SSTP_PassThru('Result',ISFUNC(reference0)||ISFUNC('On_'+reference0)||ISFUNC('SHIORI_EV.'+reference0)||ISFUNC('SHIORI_EV.On_'+reference0))
		}
		*/
		bool Has_Event(std::wstring event_name){
			return NOTYFY({{ L"Event", L"Has_Event" },{ L"Reference0", event_name }})[L"Result"]==L"1";
		}
	};
	
	#ifndef DONT_USE_SOCKET
	typedef SSTP_link_T<SSTP_link_Socket_t> SSTP_link_t;
	typedef SSTP_link_T<SSTP_link_Socket_t> SSTP_Socket_link_t;
	#endif
	typedef SSTP_link_T<SSTP_link_Direct_t> SSTP_Direct_link_t;
}

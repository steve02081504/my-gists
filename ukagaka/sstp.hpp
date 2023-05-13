#pragma once
#ifndef DONT_USE_SOCKET
#include "../socket.hpp"
#endif		 // !DONT_USE_SOCKET
#include "protocol_message.hpp"
#include <time.h>

namespace SSTP_link_n{
	using namespace ukagaka;
	#ifndef DONT_USE_SOCKET
	using namespace Socket_link_n;
	#endif		 // !DONT_USE_SOCKET
	struct SSTP_link_args_t: base_protocol_message {
		using base_protocol_message::base_protocol_message;
		bool has(std::wstring a)const{return base_protocol_message::has(L"X-SSTP-PassThru-"+a);}
		auto&operator[](std::wstring a){return base_protocol_message::operator[](L"X-SSTP-PassThru-"+a);}
		auto& base_arec(std::wstring a){return base_protocol_message::operator[](a);}

		const auto& operator[](std::wstring a) const { return base_protocol_message::operator[](L"X-SSTP-PassThru-" + a); }
		const auto& base_arec(std::wstring a) const { return base_protocol_message::operator[](a); }
	};

	struct SSTP_ret_t: protocol_message {
		using protocol_message::protocol_message;
		bool  has(std::wstring a) const { return base_protocol_message::has(L"X-SSTP-PassThru-" + a); }
		auto& operator[](std::wstring a) { return base_protocol_message::operator[](L"X-SSTP-PassThru-" + a); }
		auto& base_arec(std::wstring a) { return base_protocol_message::operator[](a); }

		const auto& operator[](std::wstring a) const { return base_protocol_message::operator[](L"X-SSTP-PassThru-" + a); }
		const auto& base_arec(std::wstring a) const { return base_protocol_message::operator[](a); }
	};

	#define dwDataOfDirectSSTP 9801

	#ifndef DONT_USE_SOCKET
	struct SSTP_link_Socket_t {
	private:
		Socket_link_t*	 pSocket = nullptr;
		SSTP_link_args_t _header{};
	protected:
		const auto& get_header()const noexcept{ return _header; }
	public:
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
				_header.base_arec(L"ReceiverGhostHWnd") = std::to_wstring((size_t)ghost);
			}
			else {
				_header.erase(L"ReceiverGhostHWnd");
			}
			return ghost;
		}
		bool link_to_ghost(std::wstring ghost) {
			if (!ghost.empty()) {
				_header.base_arec(L"ReceiverGhostName") = ghost;
			}
			else {
				_header.erase(L"ReceiverGhostName");
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
				return {};
			else
				return pSocket->base_get_ret();
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
	private:
		bool usingDirectSSTP=0;
		HWND toghost=0;
		HWND replay_to=0;
		std::string aret;
	protected:
		SSTP_link_args_t get_header()const { return {}; }
	public:
		bool was_linked_to_ghost() noexcept {
			return toghost;
		}
		void link_to_ghost(HWND ghost) noexcept {
			toghost = ghost;
		}
		void base_send(std::string massage) noexcept {
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
				return {};
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
					return {};
				else
					Sleep(250);
			}
		}
		void before_SSTP_send() noexcept {
			aret.clear();
		}
		void DirectSSTPprocess(const COPYDATASTRUCT*pcdt) {
			aret = std::string((const char*)pcdt->lpData, (size_t)pcdt->cbData);
		}
		void setReplayHwnd(HWND hwnd)noexcept {
			replay_to = hwnd;
		}
	};
	template<typename base_link_t>
	struct SSTP_link_T:base_link_t{
	private:
		SSTP_link_args_t _header;
	public:
		SSTP_link_T(
					SSTP_link_args_t header={{L"Charset",L"UTF-8"},{L"Sender",L"void"}}
					):
		_header(header){}

		SSTP_ret_t base_SSTP_send(std::wstring head,SSTP_link_args_t args){
			base_link_t::before_SSTP_send();
			{
				auto send=protocol_message{head,base_link_t::get_header()+_header+args};
				base_link_t::base_send((std::string)send);
			}
			return(SSTP_ret_t)base_link_t::base_get_ret();
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

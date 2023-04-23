#pragma once

#include <string>
#include <vector>
#include "../codepage.hpp"

namespace ukagaka {
	struct base_protocol_message {
		struct param_t{
			std::wstring name;
			std::wstring var;
		};
	private:
		std::vector<param_t> _params;
		auto&arec(std::wstring a) {
			for (auto& x : _params) {
				if (x.name == a)
					return x.var;
			}
			_params.push_back({ a,{} });
			return _params.back().var;
		}
		const auto&arec(std::wstring a)const noexcept{
			for (auto& x : _params) {
				if (x.name == a)
					return x.var;
			}
			static const std::wstring empty;
			return empty;
		}
	public:
		base_protocol_message() = default;
		base_protocol_message(std::initializer_list<param_t> a) {
			_params = a;
		}
		explicit base_protocol_message(std::wstring a) {
			if (a.find(L":") > a.find(L"\r\n"))
				a.erase(0, a.find(L"\r\n"));
			while (!a.empty()) {
				std::wstring t = a.substr(0, a.find(L"\r\n"));
				if (t.find(L": ") != t.npos) {
					arec(t.substr(0, t.find(L": "))) = t.substr(t.find(L": ") + 2);
					a.erase(0, a.find(L"\r\n") + 2);
					while (!a.substr(0, a.find(L"\r\n")).find(L": ")) {
						arec(t.substr(0, t.find(L": "))) += L"\r\n" + a.substr(0, a.find(L"\r\n"));
						a.erase(0, a.find(L"\r\n") + 2);
					}
				}
				else {
					a.erase(0, a.find(L"\r\n") + 2);
				}
			}
		}
		template<class T>
		explicit base_protocol_message(T a) {
			*this = base_protocol_message(std::wstring(a));
		}
		explicit base_protocol_message(std::string a) {
			auto charset = a;
			charset		 = charset.substr(charset.find("\r\nCharset: ") + 11);
			charset		 = charset.substr(0, charset.find("\r\n"));
			using namespace CODEPAGE_n;
			auto _wstring_this = MultiByteToUnicode(a, StringtoCodePage(charset));
			*this = base_protocol_message(_wstring_this);
		}
		explicit operator std::wstring()const {
			std::wstring aret;
			for (auto& x : _params) {
				if(x.var.size())
					aret += x.name + L": " + x.var + L"\r\n";
			}
			aret+=L"\r\n";
			return aret;
		}
		auto get_charset_codepage()const {
			auto charset = arec(L"Charset");
			using namespace CODEPAGE_n;
			return StringtoCodePage(charset);
		}
		explicit operator std::string()const {
			using namespace CODEPAGE_n;
			return UnicodeToMultiByte((std::wstring)*this, get_charset_codepage());
		}
		auto var(){return arec(L"Script");}
		auto&get_params()noexcept{return _params;}
		const auto&get_params()const noexcept{return _params;}
		bool has(std::wstring a){return arec(a).size();}
		auto&operator[](std::wstring a){return arec(a);}
		const auto&operator[](std::wstring a)const{return arec(a);}
		auto erase(std::wstring a)noexcept{
			for(auto i=_params.begin();i!=_params.end();i++){
				if(i->name==a){
					_params.erase(i);
					return;
				}
			}
		}
		auto&operator+=(const base_protocol_message&a)&{
			_params.insert(_params.end(),a._params.begin(),a._params.end());
			return *this;
		}
		base_protocol_message operator+(const base_protocol_message&a)const&{
			auto aret=*this;
			aret+=a;
			return aret;
		}
		base_protocol_message&&operator+(const base_protocol_message&a)&&{
			return std::move(*this+=a);
		}
	};

	template<class T>
	auto&&operator<<(T&&a,const base_protocol_message&b) {return a<<std::wstring(b);}
	inline std::string to_string(const base_protocol_message&b) {return std::string(b);}
	std::string to_ansi_colored_string(const base_protocol_message&b);
	inline std::wstring to_wstring(const base_protocol_message&b) {return std::wstring(b);}
	std::wstring to_ansi_colored_wstring(const base_protocol_message&b);

	struct protocol_message : base_protocol_message {
		std::wstring _head;
		explicit protocol_message(std::wstring a) {
			_head = a.substr(0, a.find(L"\r\n"));
			a.erase(0, _head.size() + 2);
			((base_protocol_message&)*this) = base_protocol_message(a);
		}
		explicit protocol_message(std::string a) {
			if(!a.size())
				return;
			auto charset = a;
			charset		 = charset.substr(charset.find("\r\nCharset: ") + 11);
			charset		 = charset.substr(0, charset.find("\r\n"));
			using namespace CODEPAGE_n;
			auto _wstring_this = MultiByteToUnicode(a, StringtoCodePage(charset));
			*this=protocol_message(_wstring_this);
		}
		protocol_message(std::wstring head,base_protocol_message base) : 
			base_protocol_message(base),_head(head){}
		explicit operator std::wstring()const {
			return _head + L"\r\n" + (std::wstring)base_protocol_message::operator std::wstring();
		}
		explicit operator std::string()const {
			using namespace CODEPAGE_n;
			return UnicodeToMultiByte((std::wstring)*this, get_charset_codepage());
		}
		auto get_head(){return _head;}
		auto get_code(){
			//将head按空格分割，取第2个
			auto head = _head;
			auto code_flag = head.find(L" ");
			if(code_flag == std::wstring::npos)
				return (long long)-1;
			head.erase(0, head.find(L" ") + 1);
			code_flag = head.find(L" ");
			if(code_flag == std::wstring::npos)
				return (long long)-1;
			head = head.substr(0, head.find(L" "));
			return std::wcstoll(head.c_str(),NULL,10);
		}
		auto&operator+=(const base_protocol_message&a)&{
			base_protocol_message::operator+=(a);
			return *this;
		}
		protocol_message operator+(const base_protocol_message&a)const&{
			auto aret=*this;
			aret+=a;
			return aret;
		}
		protocol_message&&operator+(const base_protocol_message&a)&&{
			return std::move(*this+=a);
		}
	};

	template<class T>
	auto&&operator<<(T&&a,const protocol_message&b) {return a<<std::wstring(b);}
	inline std::string to_string(const protocol_message&b) {return std::string(b);}
	std::string to_ansi_colored_string(const protocol_message&b);
	inline std::wstring to_wstring(const protocol_message&b) {return std::wstring(b);}
	std::wstring to_ansi_colored_wstring(const protocol_message&b);
}

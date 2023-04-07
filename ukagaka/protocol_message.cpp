#pragma once
#include "protocol_message.hpp"
#include "../ansi_color.hpp"

namespace ukagaka{
	std::wstring to_ansi_colored_wstring(const base_protocol_message&b){
		std::wstring aret;
		const auto&params = b.get_params();
		for(const auto&i:params)
			aret+=L"" SET_GREEN+i.name+L"" SET_WHITE ": " SET_CYAN +i.var+L"\r\n";
		aret += L"\r\n" RESET_COLOR;
		return aret;
	}
	std::string to_ansi_colored_string(const base_protocol_message&b){
		auto charset=b.get_charset_codepage();
		return UnicodeToMultiByte(to_ansi_colored_wstring(b),charset);
	}
	std::wstring to_ansi_colored_wstring(const protocol_message&b){
		std::wstring aret;
		aret+=L"" SET_LIGHT_YELLOW+b._head+L"\r\n" RESET_COLOR;
		aret+=to_ansi_colored_wstring((const base_protocol_message&)b);
		return aret;
	}
	std::string to_ansi_colored_string(const protocol_message&b){
		auto charset=b.get_charset_codepage();
		return UnicodeToMultiByte(to_ansi_colored_wstring(b),charset);
	}
}

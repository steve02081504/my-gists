#pragma once
#include <string>

std::string to_json_string(std::wstring str) {
	//对每个不在通用字符平面的字符进行转义
	//\uXXXX
	std::string escaped;
	for (auto& c : str) {
		if (c < 0x10000) {
			switch(c) {
			case L'\\':
				escaped += "\\\\";
				break;
			case L'"':
				escaped += "\\\"";
				break;
			case L'\b':
				escaped += "\\b";
				break;
			case L'\f':
				escaped += "\\f";
				break;
			case L'\n':
				escaped += "\\n";
				break;
			case L'\r':
				escaped += "\\r";
				break;
			case L'\t':
				escaped += "\\t";
				break;
			default:
				escaped += (char)c;
			}
		}
		else {
			escaped += "\\u";
			escaped += ((c >> 12)&0xF)+ '0';
			escaped += ((c >> 8) &0xF)+ '0';
			escaped += ((c >> 4) &0xF)+ '0';
			escaped += ((c >> 0) &0xF)+ '0';
		}
	}
	return escaped;
}

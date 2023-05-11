#pragma once
#define NOMINMAX
#include <windows.h>

#include <string>
#include <unordered_map>

//由于unicode标准时刻在变化，今天的单字节宽度字符可能明天变成了双字节宽度字符，所以我们通过运行时创建一个终端窗口来获取字符宽度
class CharWidthMap_t{
	std::unordered_map<wchar_t, uint8_t> _map;
	//运行时创建一个终端窗口来获取字符宽度
	HANDLE _h;
public:
	CharWidthMap_t()noexcept{
		//创建一个终端窗口
		_h = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	}
	~CharWidthMap_t()noexcept{
		CloseHandle(_h);
	}
	uint8_t operator[](const wchar_t ch)noexcept{
		auto it = _map.find(ch);
		if(it != _map.end()){
			return it->second;
		}
		//获取字符宽度
		DWORD written;
		WriteConsoleW(_h, &ch, 1, &written, nullptr);
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(_h, &info);
		_map[ch] = (uint8_t)info.dwCursorPosition.X;
		//重设光标位置
		SetConsoleCursorPosition(_h, {0, 0});
		return (uint8_t)info.dwCursorPosition.X;
	}
};
inline CharWidthMap_t CharWidthMap;

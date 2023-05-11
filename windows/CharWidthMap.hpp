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
		//二分法设置窗口大小，使得窗口宽度最大，长度最小
		//这里的窗口长宽是字符宽度，而不是像素宽度
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(_h, &info);
		size_t left = 0, right = info.dwSize.Y;
		while(left < right){
			size_t mid = (left + right) / 2;
			COORD size = {info.dwSize.X, (SHORT)mid};
			if(SetConsoleScreenBufferSize(_h, size)){//设置成功
				if(mid == right)
					break;
				right = mid;
			}else//设置失败
				left = mid + 1;
		}
		GetConsoleScreenBufferInfo(_h, &info);
		left = 0, right = -1;
		while(left < right){
			size_t mid = (left + right) / 2;
			COORD size = {(SHORT)mid, info.dwSize.Y};
			if(SetConsoleScreenBufferSize(_h, size)){//设置成功
				if(mid == left)
					break;
				left = mid;
			}else//设置失败
				right = mid - 1;
		}
	}
	~CharWidthMap_t()noexcept{
		CloseHandle(_h);
	}
	uint8_t operator[](const wchar_t ch)noexcept{
		auto it = _map.find(ch);
		if(it != _map.end())
			return it->second;
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
	size_t operator[](const std::wstring& str)noexcept{
		return (*this)[std::wstring_view(str)];
	}
	size_t operator[](const std::wstring_view str)noexcept{
		if (str.empty())
			return 0;
		else if (str.size() == 1)
			return (*this)[str[0]];
		//获取字符串宽度
		DWORD written;
		WriteConsoleW(_h, str.data(), (DWORD)str.size(), &written, nullptr);
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(_h, &info);
		//重设光标位置
		SetConsoleCursorPosition(_h, {0, 0});
		if(info.dwCursorPosition.Y){
			//二分法处理
			size_t split = str.size() / 2;
			std::wstring_view left = str.substr(0, split);
			std::wstring_view right = str.substr(split);
			return (*this)[left] + (*this)[right];
		}
		return (size_t)info.dwCursorPosition.X;
	}
	size_t operator[](const wchar_t* str)noexcept{
		return (*this)[std::wstring_view(str)];
	}
};
inline CharWidthMap_t CharWidthMap;

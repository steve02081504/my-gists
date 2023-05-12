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
			const size_t mid = (left + right) / 2;
			const COORD	 size = {info.dwSize.X, (SHORT)mid};
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
			const size_t mid	 = (left + right) / 2;
			const COORD	 size = {(SHORT)mid, info.dwSize.Y};
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
		const auto it = _map.find(ch);
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
			const size_t split = str.size() / 2;
			const std::wstring_view left = str.substr(0, split);
			const std::wstring_view	right = str.substr(split);
			return (*this)[left] + (*this)[right];
		}
		return (size_t)info.dwCursorPosition.X;
	}
	size_t operator[](const wchar_t* str)noexcept{
		return (*this)[std::wstring_view(str)];
	}
	COORD GetPosesOfStr(std::wstring_view str, SHORT width, COORD base_pos, auto for_each_pos) noexcept {
		auto&pos=base_pos;
		//计算最后一行的位置
		for(auto ch: str) {
			if(ch == '\n') {
				pos.X = 0;
				pos.Y++;
			}
			else if(ch == '\t') {
				SetConsoleCursorPosition(_h, {pos.X, 0});
				CONSOLE_SCREEN_BUFFER_INFO info;
				WriteConsoleW(_h, L"\t", 1, nullptr, nullptr);
				GetConsoleScreenBufferInfo(_h, &info);
				pos.X = info.dwCursorPosition.X;
			}
			else {
				const auto ch_width = (*this)[ch];
				if(pos.X + ch_width >= width) {
					pos.X = pos.X + ch_width == width ? 0 : ch_width;
					pos.Y++;
				}
				else
					pos.X += ch_width;
			}
			for_each_pos(pos);
		}
		return pos;
	}
	[[nodiscard]]COORD GetPosAfterStr(std::wstring_view str, SHORT width, COORD base_pos = {0, 0}) noexcept {
		return GetPosesOfStr(str, width, base_pos, [](auto) noexcept {});
	}
};
inline CharWidthMap_t CharWidthMap;

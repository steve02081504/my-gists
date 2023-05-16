#include <string>
#include <vector>
#include <ranges>

#define NOMINMAX
#include <windows.h>

#include "CharWidthMap.hpp"
#include "../STL/replace_all.hpp"
#include "small_io.hpp"//small_io
#include "clipboard.cpp"//setClipboard、getClipboard
#include "shell_base.hpp"

using namespace std;

#define floop while(1)

constexpr inline bool operator==(const COORD& l, const COORD& r) noexcept {
	return l.X == r.X && l.Y == r.Y;
}
namespace terminal_n{
	class terminal_runner{
		inline static vector<terminal*> active_terminals;

		struct edit_history_t{
			vector<editting_command_t> commands{{L"",0}};
			size_t index=0;
			editting_command_t undo(){
				if(index)
					index--;
				return commands[index];
			}
			editting_command_t redo(){
				if(index<commands.size()-1)
					index++;
				return commands[index];
			}
			void update(const editting_command_t&command){
				if(command.command==commands[index].command)
					return;//no change
				if(index<commands.size()-1)
					commands.erase(commands.begin()+index+1,commands.end());
				commands.push_back(command);
				index=commands.size()-1;
			}
		};

		static BOOL WINAPI ConsoleHandler(DWORD CEvent){
			switch(CEvent){
				case CTRL_CLOSE_EVENT:
					while (active_terminals.size()) {
						active_terminals.back()->terminal_exit();
						active_terminals.pop_back();
					}
					break;
				default:
					break;
			}
			return TRUE;
		}

		terminal* base;
	public:
		terminal_runner(terminal*pt)noexcept:base(pt){}
		void run(size_t argc, vector<wstring>& argv) {
			SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE);
			active_terminals.push_back(base);
			base->before_terminal_login();
			base->terminal_args(argc, argv);
			base->terminal_login();

			const auto hOut=GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO CurSorInfo;
			GetConsoleCursorInfo(hOut, &CurSorInfo);
			auto showCursor = [&]() noexcept {
				CurSorInfo.bVisible = TRUE;
				SetConsoleCursorInfo(hOut, &CurSorInfo);
			};
			auto hideCursor = [&]() noexcept {
				CurSorInfo.bVisible = FALSE;
				SetConsoleCursorInfo(hOut, &CurSorInfo);
			};
			wstring muti_line_command;

			INPUT_RECORD irInBuf;
			HANDLE		 hIn = GetStdHandle(STD_INPUT_HANDLE);
			DWORD		 cNumRead;

			floop{
				wstring command_prompt=base->terminal_command_prompt();
				out << command_prompt;

				base->terminal_command_history_new();
				edit_history_t edit_history;
				editting_command_t command;
				size_t tab_num=0;
				size_t before_history_index=0;
				
				reprinter_t reprinter;
				reprinter.set_prompt(command_prompt);

				auto move_insert_index=[&](ptrdiff_t move_size)noexcept{
					if(!move_size)return;
					reprinter.move_curour(command.insert_index,move_size);
					command.insert_index += move_size;
				};
				auto reflash_command=[&](const editting_command_t&new_command){
					//重绘新的命令
					auto command_for_show=base->terminal_command_update(new_command.command);
					reprinter.update_content(new_command,command_for_show);
					command=new_command;
				};
				if(muti_line_command.size()){
					const auto line_break_pos=muti_line_command.find(L'\n');
					if(line_break_pos!=wstring::npos){
						command.command=muti_line_command.substr(0,line_break_pos);
						muti_line_command.erase(0,line_break_pos+1);
						command.insert_index=line_break_pos;
						reflash_command(command);
						
						out << L'\n';
						goto run_command;
					}
					else{
						command.command=muti_line_command;
						command.insert_index=muti_line_command.size();
						muti_line_command.clear();
						reflash_command(command);
					}
				}
				floop{
					showCursor();
					if(!ReadConsoleInputW(hIn, &irInBuf, 1, &cNumRead)){
						if(GetFileType(hIn)==FILE_TYPE_DISK){//eof i guess?
							if(command.empty())
								goto end;
							else
								goto run_command;
						}
						else
							continue;
					}
					hideCursor();
					switch(irInBuf.EventType){
						case KEY_EVENT:
							break;
						case WINDOW_BUFFER_SIZE_EVENT:
							reprinter.window_resize(irInBuf.Event.WindowBufferSizeEvent.dwSize,command.insert_index);
							[[fallthrough]];
						default:
							//ignore
							continue;
					}
					if(irInBuf.Event.KeyEvent.bKeyDown==FALSE)
						continue;
					const auto&key=irInBuf.Event.KeyEvent;
					constexpr auto ctrl_pressed=LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED;
					//esc
					if(key.wVirtualKeyCode==VK_ESCAPE)
						goto end;
					//enter
					if(key.wVirtualKeyCode==VK_RETURN){
						out << L'\n';
						goto run_command;
					}
					//ctrl-x
					if(key.wVirtualKeyCode==0x58&&key.dwControlKeyState&ctrl_pressed){
						setClipboard(command.command);
						command.clear();
					}
					//ctrl-z Undo
					if(key.wVirtualKeyCode==0x5A&&key.dwControlKeyState&ctrl_pressed)
						reflash_command(edit_history.undo());
					//ctrl-y redo
					if(key.wVirtualKeyCode==0x59&&key.dwControlKeyState&ctrl_pressed)
						reflash_command(edit_history.redo());
					//ctrl-c
					if(key.wVirtualKeyCode==0x43&&key.dwControlKeyState&ctrl_pressed)
						setClipboard(command.command);
					//ctrl-v
					if(key.wVirtualKeyCode==0x56&&key.dwControlKeyState&ctrl_pressed){
						auto insert_text=getClipboard();
						auto new_command=command.insert(insert_text);
						replace_all(new_command.command,L"\r\n"sv,L"\n"sv);
						replace_all(new_command.command,L"\r"sv,L"\n"sv);
						auto line_break=new_command.command.find(L'\n');
						if(line_break!=wstring::npos){
							muti_line_command=new_command.command.substr(line_break+1);
							new_command.command=new_command.command.substr(0,line_break);
						}
						reflash_command(new_command);
						if(muti_line_command.size()){
							out << L'\n';
							goto run_command;
						}
					}
					//tab
					if(key.wVirtualKeyCode==VK_TAB){
						tab_num+=key.wRepeatCount-1;
						auto new_command=base->terminal_tab_press(command,tab_num);
						reflash_command(new_command);
						tab_num++;
					}
					else
						tab_num=0;
					//up
					if(key.wVirtualKeyCode==VK_UP){
						base->terminal_command_history_update(command.command, before_history_index);
						if(base->terminal_command_history_forward(before_history_index,key.wRepeatCount))
							reflash_command(base->terminal_get_command_history(before_history_index));
					}
					//down
					if(key.wVirtualKeyCode==VK_DOWN)
						if(before_history_index) {
							base->terminal_command_history_update(command.command, before_history_index);
							if(before_history_index>=key.wRepeatCount)
								before_history_index-=key.wRepeatCount;
							else
								before_history_index=0;
							reflash_command(base->terminal_get_command_history(before_history_index));
						}
					//left
					if(key.wVirtualKeyCode==VK_LEFT)
						if(key.dwControlKeyState&ctrl_pressed){
							if(command.insert_index)
								reflash_command(base->terminal_quick_move_left(command,key.wRepeatCount));
						}
						else
							if(command.insert_index>=key.wRepeatCount)
								move_insert_index(-(ptrdiff_t)key.wRepeatCount);
							else
								move_insert_index(-(ptrdiff_t)command.insert_index);
					//right
					if(key.wVirtualKeyCode==VK_RIGHT)
						if(key.dwControlKeyState&ctrl_pressed){
							if(command.insert_index==command.command.size())
								reflash_command(base->terminal_command_complete_by_right(command));
							else
								reflash_command(base->terminal_quick_move_right(command,key.wRepeatCount));
						}
						else
							if(command.insert_index+key.wRepeatCount<=command.command.size())
								move_insert_index(+(ptrdiff_t)key.wRepeatCount);
							else{
								move_insert_index(command.command.size()-command.insert_index);
								reflash_command(base->terminal_command_complete_by_right(command));
							}
					//delete
					if(key.wVirtualKeyCode==VK_DELETE)
						if(key.dwControlKeyState&ctrl_pressed){
							if(command.insert_index<command.command.size())
								reflash_command(base->terminal_quick_no_move_delete(command,key.wRepeatCount));
						}
						else
							reflash_command(command.erase_with_no_move(key.wRepeatCount));
					//backspace
					if(key.wVirtualKeyCode==VK_BACK)
						if(key.dwControlKeyState&ctrl_pressed){
							if(command.insert_index)
								reflash_command(base->terminal_quick_delete(command,key.wRepeatCount));
						}
						else
							reflash_command(command.erase(key.wRepeatCount));
					//others!
					if(key.uChar.UnicodeChar && !(key.dwControlKeyState&ctrl_pressed)){
						const auto&c=key.uChar.UnicodeChar;
						if(CharWidthMap[c]==0 || key.wVirtualKeyCode==VK_TAB)
							continue;
						reflash_command(command.insert(c,key.wRepeatCount));
					}
					edit_history.update(command);
				}
			run_command:
				hideCursor();
				base->terminal_command_history_update(command.command, 0);
				if(!base->terminal_run(command.command))
					break;
			}
		end:
			showCursor();
			base->terminal_exit();
			active_terminals.pop_back();
			SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,FALSE);
		}
	};
	void terminal::base_main(size_t argc, vector<wstring>&argv){
		terminal_runner(this).run(argc,argv);
	}

	void terminal::before_terminal_login() {
		if(this->enable_virtual_terminal_processing()) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleMode(hOut, &old_mode_out);
			DWORD dwMode = old_mode_out | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			SetConsoleMode(hOut, dwMode);
			HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
			GetConsoleMode(hErr, &old_mode_err);
			dwMode = old_mode_err | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			SetConsoleMode(hErr, dwMode);
		}
	}

	void terminal::terminal_exit() {
		if(this->enable_virtual_terminal_processing()) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleMode(hOut, old_mode_out);
			HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
			SetConsoleMode(hErr, old_mode_err);
		}
	}

	editting_command_t terminal::terminal_quick_move_left(const editting_command_t& command, size_t move_num) {
		auto new_command = command;
		while(move_num-- && new_command.insert_index) {
			size_t new_index = new_command.find_forward_diff_ch_type();
			if(new_index != new_command.insert_index)
				new_command.insert_index = new_index;
			else
				new_command.insert_index--;
		}
		return new_command;
	}

	editting_command_t terminal::terminal_quick_move_right(const editting_command_t& command, size_t move_num) {
		auto new_command = command;
		while(move_num-- && new_command.insert_index < new_command.command.size()) {
			size_t new_index = new_command.find_backward_diff_ch_type();
			if(new_index != new_command.insert_index)
				new_command.insert_index = new_index;
			else
				new_command.insert_index++;
		}
		return new_command;
	}

	editting_command_t terminal::terminal_quick_no_move_delete(const editting_command_t& command, size_t delete_num) {
		auto new_command = command;
		while(delete_num-- && new_command.insert_index < new_command.command.size()) {
			size_t new_index = new_command.find_backward_diff_ch_type();
			if(new_index != new_command.insert_index)
				new_command.insert_index = new_index;
			else
				new_command.insert_index++;
		}
		return command.erase_with_no_move(new_command.insert_index - new_command.size());
	}

	editting_command_t terminal::terminal_quick_delete(const editting_command_t& command, size_t delete_num) {
		auto new_command = command;
		while(delete_num-- && new_command.insert_index) {
			size_t new_index = new_command.find_forward_diff_ch_type();
			if(new_index != new_command.insert_index)
				new_command.insert_index = new_index;
			else
				new_command.insert_index--;
		}
		return command.erase(new_command.size() - new_command.insert_index);
	}

	inline editting_command_t&& editting_command_t::insert(const wstring&insert_str) && {
		command.insert(insert_index, insert_str);
		insert_index += insert_str.size();
		return move(*this);
	}

	inline editting_command_t editting_command_t::insert(const wstring&insert_str) const& {
		auto ret = *this;
		move(ret).insert(insert_str);
		return ret;
	}

	inline editting_command_t&& editting_command_t::insert(wchar_t insert_char) && {
		command.insert(insert_index, 1, insert_char);
		insert_index++;
		return move(*this);
	}

	inline editting_command_t editting_command_t::insert(wchar_t insert_char) const& {
		auto ret = *this;
		move(ret).insert(insert_char);
		return ret;
	}

	inline editting_command_t&& editting_command_t::insert(wchar_t insert_char,size_t repeat_count) && {
		command.insert(insert_index, repeat_count, insert_char);
		insert_index+=repeat_count;
		return move(*this);
	}

	inline editting_command_t editting_command_t::insert(wchar_t insert_char,size_t repeat_count) const& {
		auto ret = *this;
		move(ret).insert(insert_char,repeat_count);
		return ret;
	}

	inline editting_command_t&& editting_command_t::erase(size_t erase_size) && {
		if(erase_size > insert_index)
			erase_size = insert_index;
		insert_index -= erase_size;
		command.erase(insert_index, erase_size);
		return move(*this);
	}

	inline editting_command_t editting_command_t::erase(size_t erase_size) const& {
		auto ret = *this;
		move(ret).erase(erase_size);
		return ret;
	}

	inline editting_command_t&& editting_command_t::erase_with_no_move(size_t erase_size) && {
		if(erase_size > command.size())
			erase_size = command.size();
		insert_index = min(insert_index, command.size() - erase_size);
		command.erase(insert_index, erase_size);
		return move(*this);
	}

	inline editting_command_t editting_command_t::erase_with_no_move(size_t erase_size) const& {
		auto ret = *this;
		move(ret).erase_with_no_move(erase_size);
		return ret;
	}

	inline size_t editting_command_t::find_backward(auto finder) {
		const auto begin = insert_index;
		const auto end	 = command.size();
		for(size_t i = begin; i != end; i++)
			if(finder(command[i]))
				return i;
		return end;
	}

	inline size_t editting_command_t::find_forward(auto finder) {
		const auto begin = insert_index;
		const auto end	 = 0;
		for(size_t i = begin; i != end; i--)
			if(finder(command[i - 1]))
				return i;
		return end;
	}

	inline wchar_t editting_command_t::backward_char() {
		if(insert_index == command.size())
			return command.back();
		return command[insert_index];
	}

	inline wchar_t editting_command_t::forward_char() {
		if(insert_index == 0)
			return command.front();
		return command[insert_index - 1];
	}

	inline size_t editting_command_t::find_backward_diff_ch_type() {
		if(command.empty())
			return 0;
		const auto ch		 = backward_char();
		size_t	   new_index = insert_index;
		if(__iswcsym(ch))
			new_index = find_backward([](wchar_t ch) {
				return !__iswcsym(ch);
			});
		else if(iswspace(ch))
			new_index = find_backward([](wchar_t ch) {
				return !iswspace(ch);
			});
		else if(iswpunct(ch))
			new_index = find_backward([](wchar_t ch) {
				return !iswpunct(ch);
			});
		return new_index;
	}

	inline size_t editting_command_t::find_forward_diff_ch_type() {
		if(command.empty())
			return 0;
		const auto ch		 = forward_char();
		size_t	   new_index = insert_index;
		if(__iswcsym(ch))
			new_index = find_forward([](wchar_t ch) {
				return !__iswcsym(ch);
			});
		else if(iswspace(ch))
			new_index = find_forward([](wchar_t ch) {
				return !iswspace(ch);
			});
		else if(iswpunct(ch))
			new_index = find_forward([](wchar_t ch) {
				return !iswpunct(ch);
			});
		return new_index;
	}

	inline base_reprinter_t::base_reprinter_t() noexcept {
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hOut, &BufferInfo);
		start_pos = BufferInfo.dwCursorPosition;
		end_pos	  = start_pos;
	}

	inline void base_reprinter_t::operator()(const wstring& str) noexcept {
		//移动光标到选项开始位置
		SetConsoleCursorPosition(hOut, start_pos);
		//输出选项
		out << str;
		//临时变量保存此时的光标位置
		const auto temp_pos = get_cursor_pos();
		if(temp_pos.Y < end_pos.Y || (temp_pos.Y == end_pos.Y && temp_pos.X <= end_pos.X)) {
			//通过窗口大小和位置，计算出需要几个空格来覆盖原来的命令的尾
			const size_t space_num = (end_pos.Y - temp_pos.Y) * BufferInfo.dwSize.X + end_pos.X - temp_pos.X;
			//输出空格
			out << X_times(space_num, L' ');
		}
		//更新结束位置
		end_pos = temp_pos;
	}
	void pos_recorder_t::rebuild_poses(size_t start) noexcept {
		COORD start_pos;
		if(start) {
			start_pos = _poses[start];
			_poses.resize(start);
		}
		else {
			start_pos = _prompt_pos;
			_poses.clear();
		}
		new_pos(start_pos);
		CharWidthMap.GetPosesOfStr(wstring_view(_content).substr(start), _width, start_pos,
								   [this](COORD pos) {
									   new_pos(pos);
								   });
	}
	inline pos_recorder_t::pos_recorder_t() noexcept {
		auto					   hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hOut, &csbi);
		_width = csbi.dwSize.X;
		rebuild_poses();
	}
	inline void pos_recorder_t::update_buffer_width(SHORT width) noexcept {
		if(_width != width) {
			_width		= width;
			_prompt_pos = CharWidthMap.GetPosAfterStr(_prompt, width);
			rebuild_poses();
		}
	}
	inline void pos_recorder_t::set_prompt(const wstring& prompt) noexcept {
		if(_prompt != prompt) {
			_prompt	   = prompt;
			const auto new_pos = CharWidthMap.GetPosAfterStr(prompt, _width);
			if(_prompt_pos != new_pos) {
				_prompt_pos = new_pos;
				rebuild_poses();
			}
		}
	}
	inline void pos_recorder_t::set_content(const wstring& command) noexcept {
		//找出相同的前缀
		const size_t same_prompt = ranges::mismatch(_content, command).in1 - _content.begin();
		_content				 = command;
		rebuild_poses(same_prompt);
	}
}		// namespace terminal_n

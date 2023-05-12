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
#include <conio.h>

using namespace std;

#define floop while(1)

bool operator==(const COORD&l,const COORD&r)noexcept{
	return l.X==r.X&&l.Y==r.Y;
}

class command_pos_recorder{
	vector<COORD> _poses;
	
	wstring _command_prompt;
	COORD _prompt_pos={0,0};
	wstring _command;
	SHORT _width;
	void new_pos(COORD pos){
		_poses.push_back(pos);
	}
	void rebuild_poses(size_t start=0)noexcept{
		COORD start_pos;
		if(start){
			start_pos = _poses[start];
			_poses.resize(start);
		}else{
			start_pos = _prompt_pos;
			_poses.clear();
		}
		new_pos(start_pos);
		CharWidthMap.GetPosesOfStr(wstring_view(_command).substr(start), _width, start_pos,
									[this](COORD pos) {
										new_pos(pos);
									});
	}
public:
	command_pos_recorder()noexcept{
		auto hOut=GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hOut,&csbi);
		_width=csbi.dwSize.X;
		rebuild_poses();
	}
	void update_buffer_width(SHORT width)noexcept{
		if(_width!=width){
			_width=width;
			_prompt_pos=CharWidthMap.GetPosAfterStr(_command_prompt,width);
			rebuild_poses();
		}
	}
	COORD get_pos_of(size_t index)noexcept{
		return _poses[index];
	}
	COORD get_pos_of(size_t index,COORD base_pos)noexcept{
		const auto pos=get_pos_of(index);
		return {
			base_pos.X+pos.X-_prompt_pos.X,
			base_pos.Y+pos.Y-_prompt_pos.Y
		};
	}
	COORD get_base_pos_from(size_t index_now,COORD pos_now)noexcept{
		const auto pos=get_pos_of(index_now);
		return {
			pos_now.X-pos.X+_prompt_pos.X,
			pos_now.Y-pos.Y+_prompt_pos.Y
		};
	}
	COORD get_end_pos_from(COORD base_pos)noexcept{
		const auto pos=_poses.back();
		return {
			base_pos.X+pos.X-_prompt_pos.X,
			base_pos.Y+pos.Y-_prompt_pos.Y
		};
	}
	void set_command_prompt(const wstring&prompt)noexcept{
		if(_command_prompt!=prompt){
			_command_prompt=prompt;
			const auto new_pos=CharWidthMap.GetPosAfterStr(prompt,_width);
			if(_prompt_pos!=new_pos){
				_prompt_pos=new_pos;
				rebuild_poses();
			}
		}
	}
	void set_command(const wstring&command)noexcept{
		//找出相同的前缀
		const size_t same_prompt = std::ranges::mismatch(_command, command).in1 - _command.begin();
		_command=command;
		rebuild_poses(same_prompt);
	}
};
class command_reprinter{
	terminal::reprinter_t reprinter;
	command_pos_recorder pos_recorder;
	void update_infos(size_t index_now)noexcept{
		const COORD pos_now=reprinter.get_cursor_pos();
		//更新终端宽度
		pos_recorder.update_buffer_width(reprinter.get_buffer_width());
		//更新终端起始点和终点
		reprinter.set_start_pos(pos_recorder.get_base_pos_from(index_now,pos_now));
		reprinter.set_end_pos(pos_recorder.get_end_pos_from(reprinter.get_start_pos()));
	}
public:
	void set_command_prompt(const wstring&prompt)noexcept{
		pos_recorder.set_command_prompt(prompt);
	}
	void move_curour(size_t index_now,ptrdiff_t move)noexcept{
		update_infos(index_now);
		reprinter.move_to(pos_recorder.get_pos_of(index_now+move,reprinter.get_start_pos()));
	}
	void update_command(editting_command_t&command,const editting_command_t&new_command,const wstring&command_for_print)noexcept{
		const auto index_now = command.insert_index;
		update_infos(index_now);
		pos_recorder.set_command(new_command.command);
		reprinter(command_for_print);
		reprinter.move_to(pos_recorder.get_pos_of(new_command.insert_index,reprinter.get_start_pos()));
		command=new_command;
	}
};

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
	void run(size_t argc, std::vector<std::wstring>& argv) {
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

		floop{
			wstring command_prompt=base->terminal_command_prompt();
			out << command_prompt;

			base->terminal_command_history_new();
			edit_history_t edit_history;
			editting_command_t command;
			size_t tab_num=0;
			size_t before_history_index=0;
			
			command_reprinter reprinter;
			reprinter.set_command_prompt(command_prompt);

			auto move_insert_index=[&](ptrdiff_t move_size)noexcept{
				reprinter.move_curour(command.insert_index,move_size);
				command.insert_index += move_size;
			};
			auto reflash_command=[&](const editting_command_t&new_command){
				//重绘新的命令
				auto command_for_show=base->terminal_command_update(new_command.command);
				reprinter.update_command(command,new_command,command_for_show);
			};
			if(muti_line_command.size()){
				const auto line_break_pos=muti_line_command.find(L'\n');
				if(line_break_pos!=wstring::npos){
					command.command=muti_line_command.substr(0,line_break_pos);
					muti_line_command.erase(0,line_break_pos+1);
					command.insert_index=line_break_pos;
					reflash_command(command);
					
					_putwch('\n');
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
				const auto c=_getwch();
				hideCursor();
				switch(c){
				case 27://esc
					goto end;
				case 13://enter
					_putwch('\n');
					goto run_command;
				case WEOF:
					if(command.empty())
						goto end;
					else
						goto run_command;
				case 24://ctrl-x
					setClipboard(command.command);
					command.clear();
					break;
				case 26://ctrl-z Undo
					reflash_command(edit_history.undo());
					break;
				case 25://ctrl-y redo
					reflash_command(edit_history.redo());
					break;
				case 3://ctrl-c
					setClipboard(command.command);
					break;
				case 22:{//ctrl-v
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
						_putwch('\n');
						goto run_command;
					}
					break;
				}
				case 9:{//tab
					auto new_command=base->terminal_tab_press(command,tab_num);
					reflash_command(new_command);
					break;
				}
				case 0xE0:{//方向字符先导字符
					switch(_getwch()){
					case 72://up
						base->terminal_command_history_update(command.command, before_history_index);
						if(base->terminal_command_history_next(before_history_index))
							reflash_command(base->terminal_get_command_history(before_history_index));
						break;
					case 80://down
						if(before_history_index) {
							base->terminal_command_history_update(command.command, before_history_index);
							before_history_index--;
							reflash_command(base->terminal_get_command_history(before_history_index));
						}
						break;
					case 75://left
						if(command.insert_index)
							move_insert_index(-1);
						break;
					case 77://right
						if(command.insert_index<command.size())
							move_insert_index(+1);
						else
							reflash_command(base->terminal_command_complete_by_right(command));
						break;
					case 83://delete
						reflash_command(command.erase_with_no_move(1));
						break;
					default:
						break;
					}
					break;
				}
				case 8://backspace
					reflash_command(command.erase(1));
					break;
				default:
					if(CharWidthMap[c]==0 || c==9)//tab或者其他乱七八糟
						break;
					reflash_command(command.insert(c));
					break;
				}
				if(c==9)tab_num++;
				else tab_num=0;
				if(c!=25&&c!=26)
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
void terminal::base_main(size_t argc, std::vector<std::wstring>&argv){
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

inline editting_command_t&& editting_command_t::insert(const std::wstring&insert_str) && {
	command.insert(insert_index, insert_str);
	insert_index += insert_str.size();
	return std::move(*this);
}

inline editting_command_t editting_command_t::insert(const std::wstring&insert_str) const& {
	auto ret = *this;
	std::move(ret).insert(insert_str);
	return ret;
}

inline editting_command_t&& editting_command_t::insert(wchar_t insert_char) && {
	command.insert(insert_index, 1, insert_char);
	insert_index++;
	return std::move(*this);
}

inline editting_command_t editting_command_t::insert(wchar_t insert_char) const& {
	auto ret = *this;
	std::move(ret).insert(insert_char);
	return ret;
}

inline editting_command_t&& editting_command_t::erase(size_t erase_size) && {
	if(erase_size > insert_index)
		erase_size = insert_index;
	insert_index -= erase_size;
	command.erase(insert_index, erase_size);
	return std::move(*this);
}

inline editting_command_t editting_command_t::erase(size_t erase_size) const& {
	auto ret = *this;
	std::move(ret).erase(erase_size);
	return ret;
}

inline editting_command_t&& editting_command_t::erase_with_no_move(size_t erase_size) && {
	if(erase_size > command.size())
		erase_size = command.size();
	insert_index = std::min(insert_index, command.size() - erase_size);
	command.erase(insert_index, erase_size);
	return std::move(*this);
}

inline editting_command_t editting_command_t::erase_with_no_move(size_t erase_size) const& {
	auto ret = *this;
	std::move(ret).erase_with_no_move(erase_size);
	return ret;
}

inline terminal::reprinter_t::reprinter_t() noexcept {
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hOut, &BufferInfo);
	start_pos = BufferInfo.dwCursorPosition;
	end_pos	  = start_pos;
}

inline void terminal::reprinter_t::operator()(const std::wstring& str) noexcept {
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

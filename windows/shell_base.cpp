#include <string>
#include <vector>

#define NOMINMAX
#include <windows.h>

#include "CharWidthMap.hpp"
#include "small_io.hpp"//small_io
#include "clipboard.cpp"//setClipboard、getClipboard
#include "shell_base.hpp"
#include <conio.h>

using namespace std;

#define floop while(1)

size_t GetStrWide(const wstring&str,size_t begin=0,size_t end=wstring::npos) noexcept {
	size_t aret=0;
	wstring_view strv(str);
	if(end==wstring::npos)
		end=strv.size();
	strv=strv.substr(begin,end-begin);
	return CharWidthMap[strv];
}

class line_break_virtual_spaces_calculator {//用于处理换行时的虚拟空格
	vector<size_t> _virtual_spaces_poses;
	SHORT _buffer_width;
public:
	line_break_virtual_spaces_calculator()noexcept:_buffer_width(0){}
	line_break_virtual_spaces_calculator(const wstring&command,SHORT buffer_width,SHORT start_pos)noexcept{
		_buffer_width = buffer_width;
		//虚拟空格的产生是因为行末所剩余的空间不足以容纳下一个字符，所以需要在行末补充空格
		//以_virtual_spaces_poses记录每个虚拟空格的位置
		SHORT pos=start_pos;
		size_t str_pos = 0;
		for(auto&c:command){
			const auto w=CharWidthMap[c];
			if(w){
				if(pos+w>buffer_width)
					_virtual_spaces_poses.push_back(str_pos);
				pos+=w;
				if(pos>=buffer_width)
					pos-=buffer_width;
			}
			str_pos++;
		}
	}
	//获取在pos处有了多少个虚拟空格
	size_t get_virtual_spaces_count(size_t pos)noexcept{
		//获取_virtual_spaces_poses中小于pos的元素个数
		//我们已知_virtual_spaces_poses是有序的，所以可以使用二分查找
		size_t left=0,right=_virtual_spaces_poses.size();
		while(left<right){
			const size_t mid=(left+right)/2;
			if(_virtual_spaces_poses[mid]<pos)
				left=mid+1;
			else
				right=mid;
		}
		return left;
	}
	//获取在两个pos之间有多少个虚拟空格
	size_t get_virtual_spaces_count(size_t pos1, size_t pos2)noexcept{
		if(pos1==pos2)
			return 0;
		return get_virtual_spaces_count(pos2)-get_virtual_spaces_count(pos1);
	}
	//更新终端宽度
	void update_buffer_width(SHORT buffer_width,SHORT start_pos,const wstring&command)noexcept{
		if(_buffer_width==buffer_width)
			return;
		*this=line_break_virtual_spaces_calculator(command,buffer_width,start_pos);
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

		floop{
			out << L">> ";

			base->terminal_command_history_new();
			edit_history_t edit_history;
			editting_command_t command;
			size_t tab_num=0;
			size_t before_history_index=0;
			
			terminal::reprinter_t reprinter;
			line_break_virtual_spaces_calculator virtual_spaces_calculator;
			auto move_insert_index=[&](ptrdiff_t move_size)noexcept{
				//更新终端宽度
				virtual_spaces_calculator.update_buffer_width(reprinter.get_buffer_width(),reprinter.get_start_pos().X,command.command);
				COORD move_pos=reprinter.get_cursor_pos();
				if(move_size>0){
					if(command.insert_index+move_size>command.command.size()){
						move_pos=reprinter.get_end_pos();
						command.insert_index=command.command.size();
					}
					else{
						auto tmp=move_pos.X
								 +GetStrWide(command.command,command.insert_index,command.insert_index+move_size)
								 +virtual_spaces_calculator.get_virtual_spaces_count(command.insert_index,command.insert_index+move_size);
						while(tmp >= (USHORT)reprinter.get_buffer_width()) {
							tmp -= reprinter.get_buffer_width();
							move_pos.Y++;
						}
						move_pos.X=(SHORT)tmp;
						command.insert_index+=move_size;
					}
				}else{
					if(command.insert_index+move_size<0){
						move_pos=reprinter.get_start_pos();
						command.insert_index=0;
					}
					else{
						ptrdiff_t tmp=int(move_pos.X)
									  -GetStrWide(command.command,command.insert_index+move_size,command.insert_index)
									  -virtual_spaces_calculator.get_virtual_spaces_count(command.insert_index+move_size,command.insert_index);
						while(tmp<0){
							tmp+=reprinter.get_buffer_width();
							move_pos.Y--;
						}
						move_pos.X=(SHORT)tmp;
						command.insert_index+=move_size;
					}
				}
				SetConsoleCursorPosition(hOut,move_pos);
			};
			auto reflash_command=[&](editting_command_t new_command){
				//重绘新的命令
				reprinter(base->terminal_command_update(new_command.command));
				//移动到命令开始的位置
				reprinter.move_to_start();
				//更新变量
				command=new_command;
				command.insert_index=0;//现在光标在命令的最前面
				//重新计算虚拟空格
				virtual_spaces_calculator=line_break_virtual_spaces_calculator(new_command.command,reprinter.get_buffer_width(),reprinter.get_start_pos().X);
				//移动光标到正确的位置
				move_insert_index(new_command.insert_index);
			};
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
					reflash_command(new_command);
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
		//当光标在行尾或下行行首时，光标会莫名其妙位置一致，我们需要+1来保证完全覆写
		const size_t space_num = (end_pos.Y - temp_pos.Y) * BufferInfo.dwSize.X + end_pos.X - temp_pos.X + 1;
		//输出空格
		out << X_times(space_num, L' ');
	}
	//更新结束位置
	end_pos = temp_pos;
}

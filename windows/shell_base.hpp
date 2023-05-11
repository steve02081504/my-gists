#include <string>
#include <vector>
#define NO_MIN_MAX
#if defined(_WINSOCKAPI_)
	#include <windows.h>
#else
	#define _WINSOCKAPI_ /* Prevent inclusion of winsock.h in windows.h */
	#include <windows.h>
	#undef _WINSOCKAPI_
#endif

struct editting_command_t{
	std::wstring command;
	size_t insert_index;
	editting_command_t(std::wstring command,size_t insert_index):command(command),insert_index(insert_index){}
	editting_command_t(std::wstring command):command(command),insert_index(command.size()){}
	editting_command_t():command(L""),insert_index(0){}
	bool empty()const noexcept{return command.empty();}
	editting_command_t&& insert(std::wstring insert_str) &&;
	editting_command_t	 insert(std::wstring insert_str) const&;
	editting_command_t&& insert(wchar_t insert_char) &&;
	editting_command_t	 insert(wchar_t insert_char) const&;
	editting_command_t&& erase(size_t erase_size) &&;
	editting_command_t	 erase(size_t erase_size) const&;
	editting_command_t&& erase_with_no_move(size_t erase_size) &&;
	editting_command_t	 erase_with_no_move(size_t erase_size) const&;
	void clear()noexcept{
		command.clear();
		insert_index=0;
	}
	size_t size() const noexcept { return command.size(); }
};
class terminal{
private:
	friend class terminal_runner;
private:
	DWORD old_mode_out;
	DWORD old_mode_err;
protected:
	virtual void before_terminal_login();
	virtual void terminal_args(size_t argc, std::vector<std::wstring>&argv){}
	virtual void terminal_login(){}
	virtual editting_command_t terminal_tab_press(const editting_command_t&command,size_t tab_num){return command;}
	virtual editting_command_t terminal_command_complete_by_right(const editting_command_t&command){return command;}
	virtual bool terminal_run(const std::wstring&command)=0;
	virtual void terminal_exit();
	virtual std::wstring terminal_command_update(std::wstring command){return command;}
	virtual void terminal_command_history_new()=0;
	virtual void terminal_command_history_update(const std::wstring&command,size_t before_num)=0;
	virtual std::wstring terminal_get_command_history(size_t before_num)=0;
	virtual constexpr bool enable_virtual_terminal_processing(){return true;}
	virtual constexpr bool terminal_command_history_next(size_t&index){return false;}
private:
	void base_main(size_t argc, std::vector<std::wstring>&argv);
public:
	class reprinter_t{
		mutable CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
		//两个变量，保存选项开始和结束的位置，用于重绘
		HANDLE hOut;
		COORD start_pos,end_pos;
	public:
		reprinter_t()noexcept;
		void operator()(const std::wstring& str)noexcept;
		void move_to_start()noexcept{
			SetConsoleCursorPosition(hOut, start_pos);
		}
		COORD get_start_pos()const noexcept{
			return start_pos;
		}
		COORD get_end_pos()const noexcept{
			return end_pos;
		}
		COORD get_cursor_pos()const noexcept{
			GetConsoleScreenBufferInfo(hOut, &BufferInfo);
			return BufferInfo.dwCursorPosition;
		}
		auto get_buffer_width()const noexcept{
			return BufferInfo.dwSize.X;
		}
	};
public:
	virtual ~terminal() noexcept {}
	void operator()(size_t argc, std::vector<std::wstring>& argv) {
		base_main(argc, argv);
	}
	void operator()(size_t argc, wchar_t** argv) {
		std::vector<std::wstring> argv_t(argc);
		auto					  tmp = argc;
		while(tmp--)
			argv_t[tmp] = argv[tmp];
		operator()(argc, argv_t);
	}
	void operator()(std::vector<std::wstring>& argv) {
		operator()(argv.size(), argv);
	}
	void operator()(wchar_t** argv) {
		std::vector<std::wstring> argv_t;
		while(*argv)
			argv_t.push_back(*argv++);
		operator()(argv_t);
	}
};

class simple_terminal:public terminal{
	std::vector<std::wstring>command_history;
protected:
	virtual void before_terminal_login()override{
		terminal::before_terminal_login();
	}
	virtual void terminal_exit()override{
		command_history.clear();
		terminal::terminal_exit();
	}
	virtual bool terminal_run(const std::wstring&command)=0;
	virtual void terminal_command_history_new()override{
		command_history.push_back({});
	}
	virtual void terminal_command_history_update(const std::wstring&command,size_t before_num)override{
		if(before_num==0)
			command_history[command_history.size() - before_num - 1] = command;
	}
	virtual std::wstring terminal_get_command_history(size_t before_num)override{
		if(before_num>=command_history.size())
			return{};
		return command_history[command_history.size()-before_num-1];
	}
	virtual constexpr bool terminal_command_history_next(size_t& index)override {
		if(index==command_history.size()-1)
			return false;
		index++;
		return true;
	}
};

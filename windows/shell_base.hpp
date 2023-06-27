#include <string>
#include <vector>
#define NOMINMAX
#if defined(_WINSOCKAPI_)
	#include <windows.h>
#else
	#define _WINSOCKAPI_ /* Prevent inclusion of winsock.h in windows.h */
	#include <windows.h>
	#undef _WINSOCKAPI_
#endif

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4100)
	#pragma warning(disable : 26440)
#endif
namespace terminal_n {
	using namespace std;
	WCHAR peek_key_input() noexcept;
	COORD get_window_size_with_old_info() noexcept;
	COORD get_window_size() noexcept;
	COORD get_cursor_pos_with_old_info() noexcept;
	COORD get_cursor_pos() noexcept;
	SHORT get_buffer_height_with_old_info() noexcept;
	SHORT get_buffer_width_with_old_info() noexcept;
	SHORT get_buffer_height() noexcept;
	SHORT get_buffer_width() noexcept;
	struct editting_command_t {
		wstring command;
		size_t	insert_index;
		editting_command_t(const wstring& command, size_t insert_index):
			command(command), insert_index(insert_index) {}
		editting_command_t(const wstring& command):
			command(command), insert_index(command.size()) {}
		editting_command_t():
			command(), insert_index(0) {}
		bool				 empty() const noexcept { return command.empty(); }
		editting_command_t&& insert(const wstring& insert_str) &&;
		editting_command_t	 insert(const wstring& insert_str) const&;
		editting_command_t&& insert(wchar_t insert_char) &&;
		editting_command_t	 insert(wchar_t insert_char) const&;
		editting_command_t&& insert(wchar_t insert_char, size_t repeat_num) &&;
		editting_command_t	 insert(wchar_t insert_char, size_t repeat_num) const&;
		editting_command_t&& erase(size_t erase_size) &&;
		editting_command_t	 erase(size_t erase_size) const&;
		editting_command_t&& erase_with_no_move(size_t erase_size) &&;
		editting_command_t	 erase_with_no_move(size_t erase_size) const&;
		void				 clear() noexcept {
			command.clear();
			insert_index = 0;
		}
		size_t size() const noexcept { return command.size(); }
		size_t find_backward(auto finder);
		size_t find_forward(auto finder);

	private:
		wchar_t backward_char();
		wchar_t forward_char();

	public:
		size_t find_backward_diff_ch_type();
		size_t find_forward_diff_ch_type();
	};
	class base_reprinter_t {
		mutable CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
		//两个变量，保存选项开始和结束的位置，用于重绘
		COORD start_pos, end_pos;

	public:
		base_reprinter_t() noexcept;
		void operator()(const wstring& str) noexcept;
		void move_to(COORD pos) noexcept;
		void move_to_start() noexcept {
			move_to(start_pos);
		}
		void set_start_pos(COORD pos) noexcept {
			start_pos = pos;
		}
		void set_end_pos(COORD pos) noexcept {
			end_pos = pos;
		}
		COORD get_start_pos() const noexcept {
			return start_pos;
		}
		COORD get_end_pos() const noexcept {
			return end_pos;
		}
		COORD& get_start_pos() noexcept {
			return start_pos;
		}
		COORD& get_end_pos() noexcept {
			return end_pos;
		}
	};

	class pos_recorder_t {
		vector<COORD> _poses;

		wstring _prompt;
		COORD	_prompt_pos = {0, 0};
		wstring _content;
		SHORT	_width;
		void	new_pos(COORD pos) {
			   _poses.push_back(pos);
		}
		void rebuild_poses(size_t start = 0) noexcept;

	public:
		pos_recorder_t() noexcept;
		void  update_buffer_width(SHORT width) noexcept;
		COORD get_pos_of(size_t index) noexcept {
			return _poses[index];
		}
		COORD get_pos_of(size_t index, COORD base_pos) noexcept {
			const auto& pos = get_pos_of(index);
			return {
				base_pos.X + pos.X - _prompt_pos.X,
				base_pos.Y + pos.Y - _prompt_pos.Y};
		}
		COORD get_base_pos_from(size_t index_now, COORD pos_now) noexcept {
			const auto& pos = get_pos_of(index_now);
			return {
				pos_now.X - pos.X + _prompt_pos.X,
				pos_now.Y - pos.Y + _prompt_pos.Y};
		}
		COORD get_end_pos_from(COORD base_pos) noexcept {
			const auto& pos = _poses.back();
			return {
				base_pos.X + pos.X - _prompt_pos.X,
				base_pos.Y + pos.Y - _prompt_pos.Y};
		}
		COORD get_base_pos_from_end_pos(COORD base_pos) noexcept {
			return get_base_pos_from(_poses.size() - 1, base_pos);
		}
		void	set_prompt(const wstring& prompt) noexcept;
		void	set_content(const wstring& command) noexcept;
		wstring get_prompt() const noexcept {
			return _prompt;
		}
		wstring get_content() const noexcept {
			return _content;
		}
	};
	class reprinter_t {
		base_reprinter_t reprinter;
		pos_recorder_t	 pos_recorder;

	public:
		void set_prompt(const wstring& prompt) noexcept {
			pos_recorder.set_prompt(prompt);
		}
		void move_curour_to(size_t index) noexcept {
			reprinter.move_to(pos_recorder.get_pos_of(index, reprinter.get_start_pos()));
		}
		void move_curour(size_t index_now, ptrdiff_t move) noexcept {
			move_curour_to(index_now + move);
		}
		void update_content(const editting_command_t& new_command, const wstring& command_for_print) noexcept {
			pos_recorder.set_content(new_command.command);
			reprinter(command_for_print);

			//考虑到输出可能导致屏幕滚动，所以需要重新设定起始点和终点的Y轴偏差值
			const auto cursor_pos = get_cursor_pos_with_old_info();//base_reprinter_t::operator()会更新info，所以这里可以适当优化
			if(get_buffer_height_with_old_info() - 1 == cursor_pos.Y) {
				const auto	assume_end_pos = pos_recorder.get_end_pos_from(reprinter.get_start_pos());
				const short Y_diff		   = cursor_pos.Y - assume_end_pos.Y;
				reprinter.get_start_pos().Y += Y_diff;
				reprinter.get_end_pos().Y += Y_diff;
			}

			move_curour_to(new_command.insert_index);
		}
		void window_resize(COORD size, size_t index_now) noexcept {
			const COORD pos_now = get_cursor_pos();
			//更新终端宽度
			pos_recorder.update_buffer_width(size.X);
			//更新终端起始点和终点
			reprinter.set_start_pos(pos_recorder.get_base_pos_from(index_now, pos_now));
			reprinter.set_end_pos(pos_recorder.get_end_pos_from(reprinter.get_start_pos()));
		}
		void window_resize(size_t index_now) noexcept {
			const COORD pos_now = get_cursor_pos();
			//更新终端宽度
			pos_recorder.update_buffer_width(get_buffer_width_with_old_info());
			//更新终端起始点和终点
			reprinter.set_start_pos(pos_recorder.get_base_pos_from(index_now, pos_now));
			reprinter.set_end_pos(pos_recorder.get_end_pos_from(reprinter.get_start_pos()));
		}
		void operator()(const wstring& str) noexcept {
			//简单调用需要考虑窗口大小变化的情况
			window_resize(pos_recorder.get_content().size());	   //简单调用总假设光标在最后
			pos_recorder.set_content(str);
			reprinter(str);
		}
	};
	class terminal {
	private:
		friend class terminal_runner;

	private:
		DWORD old_mode_out;
		DWORD old_mode_err;

	protected:
		virtual void			   before_terminal_login();
		virtual void			   terminal_args(size_t argc, vector<wstring>& argv) {}
		virtual void			   terminal_login() {}
		virtual editting_command_t terminal_tab_press(const editting_command_t& command, size_t tab_num) { return command; }
		virtual editting_command_t terminal_command_complete_by_right(const editting_command_t& command) { return command; }
		virtual bool			   terminal_run(const wstring& command) = 0;
		virtual void			   terminal_exit();
		virtual wstring			   terminal_command_update(wstring command) { return command; }
		virtual wstring			   terminal_command_prompt() {
			   using namespace literals;
			   return L">> "s;
		}
		virtual void		   terminal_command_history_new()											  = 0;
		virtual void		   terminal_command_history_update(const wstring& command, size_t before_num) = 0;
		virtual wstring		   terminal_get_command_history(size_t before_num)							  = 0;
		virtual constexpr bool enable_virtual_terminal_processing() { return true; }
		virtual bool		   terminal_command_history_forward(size_t& index, size_t forward_num) { return false; }

		virtual editting_command_t terminal_quick_move_left(const editting_command_t& command, size_t move_num);
		virtual editting_command_t terminal_quick_move_right(const editting_command_t& command, size_t move_num);
		virtual editting_command_t terminal_quick_no_move_delete(const editting_command_t& command, size_t delete_num);
		virtual editting_command_t terminal_quick_delete(const editting_command_t& command, size_t delete_num);

	private:
		void base_main(size_t argc, vector<wstring>& argv);

	public:
		virtual ~terminal() noexcept {}
		void operator()(size_t argc, vector<wstring>& argv) {
			base_main(argc, argv);
		}
		void operator()(size_t argc, wchar_t** argv) {
			vector<wstring> argv_t(argc);
			auto			tmp = argc;
			while(tmp--)
				argv_t[tmp] = argv[tmp];
			operator()(argc, argv_t);
		}
		void operator()(vector<wstring>& argv) {
			operator()(argv.size(), argv);
		}
		void operator()(wchar_t** argv) {
			vector<wstring> argv_t;
			while(*argv)
				argv_t.push_back(*argv++);
			operator()(argv_t);
		}
	};

	class simple_terminal: public terminal {
		vector<wstring> command_history;

	protected:
		virtual void before_terminal_login() override {
			terminal::before_terminal_login();
		}
		virtual void terminal_exit() override {
			command_history.clear();
			terminal::terminal_exit();
		}
		virtual bool terminal_run(const wstring& command) = 0;
		virtual void terminal_command_history_new() override {
			command_history.push_back({});
		}
		virtual void terminal_command_history_update(const wstring& command, size_t before_num) override {
			if(before_num == 0)
				command_history[command_history.size() - before_num - 1] = command;
		}
		virtual wstring terminal_get_command_history(size_t before_num) override {
			if(before_num >= command_history.size())
				return {};
			return command_history[command_history.size() - before_num - 1];
		}
		virtual bool terminal_command_history_forward(size_t& index, size_t forward_num) override {
			const auto index_max = command_history.size() - 1;
			if(index == index_max)
				return false;
			index += forward_num;
			if(index > index_max)
				index = index_max;
			return true;
		}
	};
}		// namespace terminal_n
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

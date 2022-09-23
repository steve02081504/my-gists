#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include "CutSpace.hpp"
#if defined(_WIN32)
	#include "../codepage.hpp"
	#include <wininet.h>
	#pragma comment(lib, "WinInet.lib")
#endif

struct yaml_reader {
	typedef std::map<std::wstring, std::wstring> yaml_map;

	std::vector<yaml_map> _data;
	size_t				  reading_index = -1;

	bool read_line(std::wstring line);
	void reader_init();
	void read_file(const std::wstring& file_path);
	#if defined(_WIN32)
	bool read_url(const std::wstring& url);
	#endif
	yaml_map find(std::wstring key, std::wstring value);
};

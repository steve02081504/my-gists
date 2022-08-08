#include <string>
#include "SSPpath.hpp"
#include "../windows/CMDargsConverter.hpp"

class SSP_Runner {
	std::wstring _path;
	bool		 _installed;

public:
	SSP_Runner(std::wstring SSPpath=GetSSPpath()){
		_path = SSPpath;
		_installed = IsSSPinstalled(SSPpath);
	}
	~SSP_Runner()=default;
	[[nodiscard]] bool IsInstalled() const { return _installed; }
	[[nodiscard]] std::wstring GetPath() const { return _path; }
	
	bool Run(std::wstring args);
	//operator()
private:
	std::wstring args_builder() {
		return {};
	}
	std::wstring args_builder(std::wstring arg) {
		return arg;
	}
	template<typename...Args>
	std::wstring args_builder(std::wstring arg, Args&&... args) {
		return CMDargsConverter(arg) + L' ' + args_builder(args...);
	}
public:
	template<typename...Args>
	bool operator()(Args&&... args) {
		return Run(args_builder(args...));
	}
	#define self (*this)
	bool run_ghost(std::wstring name) {
		return self(L"/G", name);
	}
	#undef self
};

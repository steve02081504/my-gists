#pragma once
#include <string>
#include "../windows/CMDargsConverter.hpp"

struct EXE_Runner {
	std::wstring _path;

	EXE_Runner(std::wstring path) {
		_path = path;
	}
	~EXE_Runner() = default;
	[[nodiscard]] std::wstring GetPath() const { return _path; }

	virtual bool Base_Run(std::wstring args);
	virtual bool Base_RunAndWait(std::wstring args);

private:
	//operator()
	std::wstring args_builder() {
		return {};
	}
	std::wstring args_builder(std::wstring arg) {
		return CMDargsConverter(arg);
	}
	template<typename... Args>
	std::wstring args_builder(std::wstring arg, Args&&... args) {
		return CMDargsConverter(arg) + L' ' + args_builder(args...);
	}

public:
	template<typename... Args>
	bool operator()(Args&&... args) {
		return Base_Run(args_builder(args...));
	}
	template<typename... Args>
	bool RunAndWait(Args&&... args) {
		return Base_RunAndWait(args_builder(args...));
	}
};

#include <string>
#include "SSPpath.hpp"
#include "../windows/EXE_Runner.hpp"

struct SSP_Runner:EXE_Runner {
	bool		 _installed;

	SSP_Runner(std::wstring SSPpath = GetSSPpath()):
		EXE_Runner(SSPpath) {
		_installed = IsSSPinstalled(SSPpath);
	}
	~SSP_Runner()=default;
	[[nodiscard]] bool IsInstalled() const { return _installed; }
	
	virtual bool Run(std::wstring args)override final;
	//operator()
	using EXE_Runner::operator();
	#define self (*this)
	bool run_ghost(std::wstring name) {
		return self(L"/G", name);
	}
	bool install_nar(std::wstring file_path) {
		return self(L"/I", file_path);
	}
	bool install_ssf(std::wstring file_path) {
		return self(L"/S", file_path);
	}
	bool send_x_ukaboot_ssp(std::wstring info) {
		return self(L"/N", info);
	}
	bool send_x_ukagaka_link(std::wstring info) {
		return self(L"/M", info);
	}
	#undef self
};
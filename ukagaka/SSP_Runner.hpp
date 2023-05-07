#include <string>
#include "SSPpath.hpp"
#include "../windows/EXE_Runner.hpp"

struct SSP_Runner:EXE_Runner {
	bool		 _installed;

	SSP_Runner(const std::wstring& SSPpath = {}):
		EXE_Runner(SSPpath) {
		_installed = IsSSPinstalled(SSPpath);
	}
	~SSP_Runner()=default;
	[[nodiscard]] bool IsInstalled() const {
		if(_path.empty())
			const_cast<SSP_Runner*>(this)->reset_path();
		return _installed;
	}
	
	void reset_path(const std::wstring& SSPpath = GetSSPpath()) {
		_path = SSPpath;
		_installed = IsSSPinstalled(SSPpath);
	}
	
	virtual bool Base_Run(const std::wstring& args) noexcept override final;
	virtual bool Base_RunAndWait(const std::wstring& args) override final;
	//operator()
	using EXE_Runner::operator();
	#define self (*this)
	bool run_ghost(const std::wstring& name) noexcept {
		return self(L"/G", name);
	}
	template<typename... Args>
	bool install_nar(const std::wstring& file_path, Args&&... args) noexcept {
		return self(L"/I", file_path, std::forward<Args>(args)...);
	}
	bool install_ssf(const std::wstring& file_path) noexcept {
		return self(L"/S", file_path);
	}
	bool send_x_ukaboot_ssp(const std::wstring& info) noexcept {
		return self(L"/N", info);
	}
	bool send_x_ukagaka_link(const std::wstring& info) noexcept {
		return self(L"/M", info);
	}
	#undef self
};

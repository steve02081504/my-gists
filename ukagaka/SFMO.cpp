#include "SFMO.hpp"
#include "../codepage.hpp"

bool SFMO_t::Update_info() {
	//ベースウェア等の保持すべきアプリは代わりにCreateMutexを使う
	HANDLE hMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"SakuraUnicodeFMO");

	//FMO用Mutexに対応していないベースウェアもあるので、見つからなかったら単にスキップ
	bool isWaitSuccess = true;

	if(hMutex) {
		//INFINITEで待機すると永遠に待ち続けてGUIが止まるので適宜工夫すること
		DWORD result = WaitForSingleObject(hMutex, INFINITE);

		if(result != WAIT_OBJECT_0) {
			isWaitSuccess = false;
		}
	}

	if(isWaitSuccess) {
		//保持すべきアプリは代わりにCreateMutexを使う
		HANDLE hFMO = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"SakuraUnicode");

		if(hFMO) {
			std::byte *pDataStart = static_cast<std::byte *>(MapViewOfFile(hFMO, FILE_MAP_ALL_ACCESS, 0, 0, 0));

			if(pDataStart) {
				char *pData = (char *)(pDataStart + 4);

				std::string	 FMOinfo_r(pData);
				std::wstring FMOinfo = CODEPAGE_n::MultiByteToUnicode(FMOinfo_r, CODEPAGE_n::CP_UTF8);
				std::wstring line, ID, key, value;
				info_map.clear();

				while(FMOinfo.size()) {
					auto end = FMOinfo.find(L"\r\n");
					line	 = FMOinfo.substr(0, end);
					if(end != std::wstring::npos)
						FMOinfo = FMOinfo.substr(end + 2);
					else
						break;
					auto suber = line.find(L'.');
					ID		   = line.substr(0, suber);
					line.erase(0, suber + 1);
					suber = line.find(L'\1');
					key	  = line.substr(0, suber);
					line.erase(0, suber + 1);
					value = line;
					if(ID.size()) {
						info_map[ID].map[key] = value;
						info_map[ID].ID		  = ID;
					}
				}

				//MapViewOfFileの解除
				UnmapViewOfFile(pDataStart);
			}

			//FMOハンドルを開放
			//ベースウェア等の保持すべきアプリは開放せず持ち続けること
			CloseHandle(hFMO);
		}
	}

	if(hMutex) {
		if(isWaitSuccess) {
			//WaitForSingleObjectでMutexが非シグナル状態になるので、シグナル状態に戻す(Release)
			ReleaseMutex(hMutex);
		}

		//最後にMutexのハンドルも要らないので開放
		//ベースウェア等の保持すべきアプリは開放せず持ち続けること
		CloseHandle(hMutex);
	}
	#if defined(_MSC_VER)
		#pragma warning(push)
		#pragma warning(disable : 6001)		  //未初始化警告diss
	#endif
	return isWaitSuccess && hMutex;
	#if defined(_MSC_VER)
		#pragma warning(pop)
	#endif
}

HWND SFMO_t::wait_ghost_info_by_name(std::wstring ghost_name, size_t wait_time) {
	size_t sleep_sec = 0;
	while(1) {
		Sleep(1000);
		++sleep_sec;
		if(sleep_sec > wait_time)
			return NULL;
		if(Update_info())
			for(auto &i: info_map) {
				HWND tmp_hwnd = (HWND)wcstoll(i.second[L"hwnd"].c_str(), nullptr, 10);
				if(i.second[L"name"] == ghost_name)
					return tmp_hwnd;
			}
	}
}

HWND SFMO_t::wait_ghost_info_by_fullname(std::wstring ghost_name, size_t wait_time) {
	size_t sleep_sec = 0;
	while(1) {
		Sleep(1000);
		++sleep_sec;
		if(sleep_sec > wait_time)
			return NULL;
		if(Update_info())
			for(auto &i: info_map) {
				HWND tmp_hwnd = (HWND)wcstoll(i.second[L"hwnd"].c_str(), nullptr, 10);
				if(i.second[L"fullname"] == ghost_name)
					return tmp_hwnd;
			}
	}
}

HWND SFMO_t::wait_ghost_info_by_name_or_fullname(std::wstring ghost_name, size_t wait_time) {
	size_t sleep_sec = 0;
	while(1) {
		Sleep(1000);
		++sleep_sec;
		if(sleep_sec > wait_time)
			return NULL;
		if(Update_info())
			for(auto &i: info_map) {
				HWND tmp_hwnd = (HWND)wcstoll(i.second[L"hwnd"].c_str(), nullptr, 10);
				if(i.second[L"name"] == ghost_name || i.second[L"fullname"] == ghost_name)
					return tmp_hwnd;
			}
	}
}

std::wstring SFMO_obj_t::get_modulestate(std::wstring a) {
	std::wstring modulestate = map[L"modulestate"];
	auto		 t			 = modulestate.find(a + L':');
	if(t == std::wstring::npos)
		return {};
	modulestate = modulestate.substr(t);
	t			= modulestate.find(L',');
	if(t != std::wstring::npos)
		modulestate = modulestate.substr(0, t);
	t = modulestate.find(L':');
	if(t == std::wstring::npos)
		return {};
	return modulestate.substr(t + 1);
}

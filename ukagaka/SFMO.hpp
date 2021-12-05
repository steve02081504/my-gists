#pragma once
#include <string>
#include <map>
#include <windows.h>
#include "../codepage.hpp"
struct SFMO_obj_t{
	std::wstring ID;
	std::map<std::wstring, std::wstring> map;
	auto& operator[](std::wstring a) { return map[a]; }
};
struct SFMO_t{
	std::map<std::wstring,SFMO_obj_t> info_map;
	bool Update_info(){
		//ベースウェア等の保持すべきアプリは代わりにCreateMutexを使う
		HANDLE hMutex = OpenMutexW(MUTEX_ALL_ACCESS,FALSE,L"SakuraUnicodeFMO");

		//FMO用Mutexに対応していないベースウェアもあるので、見つからなかったら単にスキップ
		bool isWaitSuccess = true;

		if ( hMutex ) {

			//INFINITEで待機すると永遠に待ち続けてGUIが止まるので適宜工夫すること
			DWORD result = WaitForSingleObject(hMutex,INFINITE);
			
			if ( result != WAIT_OBJECT_0 ) {
				isWaitSuccess = false;
			}
		}

		if ( isWaitSuccess ) {

			//保持すべきアプリは代わりにCreateMutexを使う
			HANDLE hFMO = OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,L"SakuraUnicode");

			if ( hFMO ) {

				std::byte *pDataStart = static_cast<std::byte*>(MapViewOfFile(hFMO,FILE_MAP_ALL_ACCESS,0,0,0));

				if ( pDataStart ) {

					char *pData = (char*)(pDataStart+4);

					std::string FMOinfo_r(pData);
					std::wstring FMOinfo = CODEPAGE_n::MultiByteToUnicode(FMOinfo_r, CODEPAGE_n::CP_UTF8);
					std::wstring line,ID,key,value;
					info_map.clear();

					while (FMOinfo.size()){
						auto end=FMOinfo.find(L"\r\n");
						line=FMOinfo.substr(0,end);
						if(end!= std::wstring::npos)
							FMOinfo = FMOinfo.substr(end+2);
						else
							break;
						auto suber = line.find(L'.');
						ID = line.substr(0, suber);
						line.erase(0, suber+1);
						suber = line.find(L'\1');
						key = line.substr(0, suber);
						line.erase(0, suber + 1);
						value = line;
						if(ID.size())
							info_map[ID].map[key] = value;
					}

					//MapViewOfFileの解除
					UnmapViewOfFile(pDataStart);
				}
					
				//FMOハンドルを開放
				//ベースウェア等の保持すべきアプリは開放せず持ち続けること
				CloseHandle(hFMO);
			}
		}

		if ( hMutex ) {
			if ( isWaitSuccess ) {
			
				//WaitForSingleObjectでMutexが非シグナル状態になるので、シグナル状態に戻す(Release)
				ReleaseMutex(hMutex);
			}

			//最後にMutexのハンドルも要らないので開放
			//ベースウェア等の保持すべきアプリは開放せず持ち続けること
			CloseHandle(hMutex);
		}
		
		return isWaitSuccess && hMutex;
	}
};

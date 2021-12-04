#include <string>
#include <map>
struct SFMO_obj_t{
	std::wstring ID;
	std::map<std::wstring, std::wstring> map;
};
struct SFMO_t{
	std::map<std::wstring,SFMO_obj_t> info_map;
	bool Update_info(){
		//ベースウェア等の保持すべきアプリは代わりにCreateMutexを使う
		HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,"SakuraUnicodeFMO");

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
			HANDLE hFMO = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,"SakuraUnicode");

			if ( hFMO ) {

				wchar_t *pDataStart = static_cast<wchar_t*>(MapViewOfFile(hFMO,FILE_MAP_ALL_ACCESS,0,0,0));

				if ( pDataStart ) {

					//頭の4バイトはFMO最大サイズ。
					//文字列終端（C言語文字列のゼロ終端）とは異なるので注意。
					unsigned long length = *reinterpret_cast<unsigned long*>(pDataStart);

					wchar_t *pData = (wchar_t*)(((char*)pDataStart)+4);

					std::wstring FMOinfo(pData,length);
					std::wstring line,ID,key,value;

					do{
						auto end=FMOinfo.find(L"\r\n");
						line=FMOinfo.substr(0,end);
						auto suber = line.find(L'.');
						ID = line.substr(0, suber);
						line.erase(0, suber+1);
						suber = line.find(L'\1');
						key = line.substr(0, suber);
						line.erase(0, suber + 1);
						value = line;

						info_map[ID].map[key] = value;
					} while (line.size());

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

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <map>
#include <ctime>
#include <cstdio>
#include "../file/filematch.hpp"
#include "../windows/MD5maker.hpp"
#include "../codepage.hpp"
#include "../file/fgetstring.h"
#include "../file/dir_enum.h"
#include "../file/windows/LastWriteTime.hpp"
#include "../file/windows/GetFileSize.hpp"
#include "../windows/FILETIME2time_t.h"
namespace updatefile_n{
	using namespace std;
	struct update_file_info{
		//ghost/master/data/Game/UNO/bReverse.png4dbfeee82e8f6e476b5687c18154a571size=1732date=2021-03-21T14:33:27
		wstring name;
		wstring md5;
		uintmax_t size=0;
		wstring time=L"2012-12-21T00:00:00";
		static wstring time2str(time_t atime) {
			wchar_t buf[512];
			if (wcsftime(buf, 512, L"%FT%T", gmtime(&atime)))
				return buf;
			else
				return L"2012-12-21T00:00:00";
		};
		explicit update_file_info(wstring str) {
			name=str.substr(0,str.find(L"\1"));
			str=str.substr(str.find(L"\1")+1);
			md5 = str.substr(0, str.find(L"\1"));
			str = str.substr(str.find(L"\1") + 1);
			//size=1732date=2021-03-21T14:33:27
			str=str.substr(5);
			//1732date=2021-03-21T14:33:27
			size = (uintmax_t)wcstoll(str.substr(0,str.find(L"\1")).c_str(), nullptr, 10);
			str = str.substr(str.find(L"\1") + 6);
			//2021-03-21T14:33:27
			time = str.substr(0, str.find(L"\1"));
		}
		explicit update_file_info(wstring file,wstring filename) {
			name = filename;
			md5 = CODEPAGE_n::MultiByteToUnicode(MD5maker.get_file_md5(file),CODEPAGE_n::CP_ACP);
			size = GetFileSize(file.c_str());
			time = time2str(FILETIME2time_t(LastWriteTime(file.c_str())));
		}
		update_file_info(const update_file_info&) = default;
		update_file_info(update_file_info&&) = default;
		update_file_info() = default;
		explicit operator wstring(){
			return name+L"\1"+md5+L"\1"+L"size="+to_wstring(size)+L"\1"+L"date="+time+L"\1";
		}
		update_file_info& operator=(const update_file_info&) = default;
		update_file_info& operator=(update_file_info&&) = default;
		void update(wstring file, wstring filename) {
			auto ttime = time2str(FILETIME2time_t(LastWriteTime(file.c_str())));
			if (time != ttime) {
				time = ttime;
				name = filename;
				md5 = CODEPAGE_n::MultiByteToUnicode(MD5maker.get_file_md5(file), CODEPAGE_n::CP_ACP);
				size = GetFileSize(file.c_str());
			}
		}
		void update_with_out_time(wstring file, wstring filename) {
			name = filename;
			md5 = CODEPAGE_n::MultiByteToUnicode(MD5maker.get_file_md5(file), CODEPAGE_n::CP_ACP);
			size = GetFileSize(file.c_str());
		}
	};
	class update_file{
		CODEPAGE_n::CODEPAGE charset;
		map<wstring,update_file_info> path_map;
		filepathMatcher_n::DefaultAllMatchFilepathMatcher matcher;
		//map<update_file_info,wstring>md5_map;
	public:
		void readrules(wstring file_path) {
			matcher.clear();
			wstring str;
			auto fp=_wfopen(file_path.c_str(), L"r");
			if(fp){
				while(fgetstring(str,fp)){
					if(str.size())
						matcher.AddRule(str);
				}
				fclose(fp);
			}
			matcher.reverse();//match->ignore
		}
		void read(wstring file_path){
			path_map.clear();
			charset=CODEPAGE_n::CP_ACP;
			wstring str;
			string nstr;
			auto fp=_wfopen(file_path.c_str(), L"r");
			if(fp){
				while(fgetstring(nstr,fp)){
					str=CODEPAGE_n::MultiByteToUnicode(nstr,charset);
					auto key = str.substr(0,str.find(L","));
					str= str.substr(str.find(L",")+1);
					if (key == L"charset") {
						charset = CODEPAGE_n::StringtoCodePage(str.c_str());
					}
					if(key==L"file"){
						update_file_info v(str);
						auto k(v.name);
						path_map[k]=v;
					}
				}
				fclose(fp);
			}
		};
		void update(wstring file_path){
			for(auto it = path_map.begin(); it != path_map.end();) {
				auto filename=file_path+it->first;
				if((!matcher(filename))||_waccess(filename.c_str(),0)){
					it=path_map.erase(it);
				}
				else
					it++;
			}
			matcher.ForDir(file_path,
				[this](wstring file_path,wstring filename){
					filename= filename.substr(1);//"/ghost"->"ghost"
					wstring k(filename);
					path_map[k].update(file_path, filename);
				}
			);
		};
		void update_with_out_time(wstring file_path){
			for(auto it = path_map.begin(); it != path_map.end();) {
				auto filename=file_path+it->first;
				if((!matcher(filename))||_waccess(filename.c_str(),0)){
					it=path_map.erase(it);
				}
				else
					it++;
			}
			matcher.ForDir(file_path,
				[this](wstring file_path,wstring filename){
					filename= filename.substr(1);//"/ghost"->"ghost"
					wstring k(filename);
					path_map[k].update_with_out_time(file_path, filename);
				}
			);
		};
		void write(wstring filepath){
			auto fp = _wfopen(filepath.c_str(), L"wb");
			if (fp) {
				write(fp);
				fclose(fp);
			}
		};
		void write(FILE* fp){
			if(charset == CODEPAGE_n::CP_ACP)
				charset = CODEPAGE_n::CP_UTF8;
			fputs(("charset,"+ CODEPAGE_n::UnicodeToMultiByte(CODEPAGE_n::CodePagetoString(charset), charset)+"\r\n").c_str(),fp);
			for(auto&pair:path_map){
				fputs(("file,"+ CODEPAGE_n::UnicodeToMultiByte((wstring)pair.second, charset)+"\r\n").c_str(),fp);
			}
		};
	};
}

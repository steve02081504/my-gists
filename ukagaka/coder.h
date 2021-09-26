#include <cstdio>
#include <string>
#include "../file/fgetstring.h"
#include "../file/ChangeSuffix.h"

#undef max//fucking windows

enum coder_mode{line_mode,char_mode};

struct Aya_Coder_t{//文
	struct decoder_t{
		static void write_header(FILE*to){}
		static int cipher(const int c)
		{
			return(((c & 0x7)<< 5)|((c & 0xf8)>> 3))^ 0x5a;
		}
		static void write_ender(FILE*to){}
	};
	struct encoder_t{
		static void write_header(FILE*to){}
		static int cipher(const int c)
		{
			return(((c ^ 0x5a)<< 3)& 0xF8)|(((c ^ 0x5a)>> 5)& 0x7);
		}
		static void write_ender(FILE*to){}
	};
	static constexpr auto codedFileSuffix=L"ayc";
	static constexpr auto NoncodedFileSuffix=L"dic";
	static constexpr auto coder_mode=char_mode;
};
struct Misaka_Coder_t{//美版
	struct decoder_t{
		static void write_header(FILE*to){}
		static int cipher(const int c)
		{
			return 255-c;
		}
		static void write_ender(FILE*to){}
	};
	struct encoder_t{
		static int cipher(const int c)
		{
			return 255-c;
		}
		static void write_header(FILE*to){
			std::string_view head="// encrypted\r\n";
			for(char c:head)
				fputc(cipher(c),to);
		}
		static void write_ender(FILE*to){
			std::string_view end="\r\n";
			for(char c:end)
				fputc(cipher(c),to);
		}
	};
	static constexpr auto codedFileSuffix=L"__1";
	static constexpr auto NoncodedFileSuffix=L"txt";
	static constexpr auto coder_mode=char_mode;
};
using namespace std;
#include "../base64.h"
struct Kawari_Coder_t{//華和梨
	//using std::string;
	struct decoder_t{
		static string docryptLine(string str)
		{
			string id=str.substr(0,9);
			if(id !="!KAWA0000"&&id !="!KAWA0001")
				return str;
			str=DecodeBase64(str.substr(9));
			unsigned char key=0xcc;
			string aret;
			if(id=="!KAWA0001") {
				key=(unsigned char)str[0];
				str.erase(str.begin());//第一个字节是算出来的key值，直接跳过
			}
			aret.reserve(str.size());
			for(auto c:str)
				aret+=c^key;
			return aret;
		}
	};
	struct encoder_t{
		static string docryptLine(const string& str)
		{
			string id=str.substr(0,9);
			if(id =="!KAWA0000"||id =="!KAWA0001")
				return str;
			string aret;
			aret.reserve(str.size());
			for(auto c:str)
				aret+=c^0xcc;
			return("!KAWA0000"+EncodeBase64(aret));
		}
	};
	static constexpr auto codedFileSuffix=L"kiw";
	static constexpr auto NoncodedFileSuffix=L"kis";
	static constexpr auto coder_mode=line_mode;
};
struct Satoriya_Coder_t{//里々
	//using std::string;
	struct base{
		static string encode(const string& s){
			const char*	p=s.c_str();
			auto len=s.size();
			string aret;
			for(size_t n=0;n<len/2;++n){
				aret+=p[n];
				aret+=p[len-n-1];
			}
			if(len&1)aret+=p[len/2];
			return aret;
		}
		static string decode(const string& s){
			const char*	p=s.c_str();
			auto len=s.size();
			string aret;
			for(size_t n=0;n<len;n+=2)aret+=p[n];
			for(long long n= long long(len)-(len&1)?2:1;n>=0;n-=2)aret+=p[n];
			return aret;
		}
	};
	struct decoder_t{
		static string docryptLine(const string& str)
		{
			return base::decode(base::decode(str));
		}
	};
	struct encoder_t{
		static string docryptLine(const string& str)
		{
			return base::encode(base::encode(str));
		}
	};
	static constexpr auto codedFileSuffix=L"sat";
	static constexpr auto NoncodedFileSuffix=L"txt";
	static constexpr auto coder_mode=line_mode;
};

template<class Coder_t>
struct Runcoder_t{
	template<class do_coder_t>
	static void runcode(FILE*from,FILE*to){
		if constexpr(Coder_t::coder_mode==char_mode){
			do_coder_t::write_header(to);
			int c;
			while((c=fgetc(from))!=EOF){
				fputc(do_coder_t::cipher(c),to);
			}
			do_coder_t::write_ender(to);
		}
		else{
			string t;
			while(fgetstring(t,from)){
				fputs(do_coder_t::docryptLine(t).c_str(),to);
			}
		}
	}
	template<class do_coder_t>
	static bool RuncodeFor(std::wstring file,std::wstring to_file){
		if(file == to_file)
			to_file += L".new";
		auto fp=_wfopen(file.c_str(),Coder_t::coder_mode==char_mode?L"rb":L"r");
		if(!fp)return 0;
		auto fp2=_wfopen(to_file.c_str(),L"wb");
		if(!fp2){
			fclose(fp);
			return 0;
		}
		runcode<do_coder_t>(fp,fp2);
		fclose(fp);
		fclose(fp2);
		return 1;
	}
	static bool EncodeDic(std::wstring file){
		return RuncodeFor<Coder_t::encoder_t>(file,ChangeSuffix(file,(std::wstring_view)Coder_t::codedFileSuffix));
	}
	static bool DecodeDic(std::wstring file){
		return RuncodeFor<Coder_t::decoder_t>(file,ChangeSuffix(file,(std::wstring_view)Coder_t::NoncodedFileSuffix));
	}
};

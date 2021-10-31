#include <string>
#include <vector>
#include <functional>
#include "../STL/replace_all.hpp"
#include "../file/dir_enum.cpp"
namespace filepathMatcher_n{
	using namespace std;
	inline bool IsMatchRule(wstring s, wstring p) {
		auto allStars = [](const wstring& str, int left, int right) {
			for (int i = left; i < right; ++i) {
				if (str[i] != L'*') {
					return false;
				}
			}
			return true;
		};
		auto charMatch = [](wchar_t u, wchar_t v) {
			return u == v || v == L'?';
		};

		while (s.size() && p.size() && p.back() != L'*') {
			if (charMatch(s.back(), p.back())) {
				s.pop_back();
				p.pop_back();
			}
			else {
				return false;
			}
		}
		if (p.empty()) {
			return s.empty();
		}

		size_t sIndex = 0, pIndex = 0;
		long long int sRecord = -1, pRecord = -1;
		while (sIndex < s.size() && pIndex < p.size()) {
			if (p[pIndex] == L'*') {
				++pIndex;
				sRecord = sIndex;
				pRecord = pIndex;
			}
			else if (charMatch(s[sIndex], p[pIndex])) {
				++sIndex;
				++pIndex;
			}
			else if (sRecord != -1 && sRecord + 1 < s.size()) {
				++sRecord;
				sIndex = (size_t)sRecord;
				pIndex = (size_t)pRecord;
			}
			else {
				return false;
			}
		}
		return allStars(p, pIndex, p.size());
	}
	inline bool IsMatchRules(const wstring&s, const vector<wstring>&rules){
		for(auto&rule:rules){
			if(IsMatchRule(s,rule))
				return 1;
		}
		return 0;
	}

	template<bool default_as_all_match=true>
	class filepathMatcher_t{
		typedef filepathMatcher_t<default_as_all_match> self_t;
		vector<wstring> MatchingRules;//*.ai
		vector<wstring> NotMatchingRules;//!*.inf

		wstring PreprocessRule(wstring rule){
			replace_all(rule,L"\\",L"/");
			if(rule[0]!=L'/')
				rule=L"*/"+rule;
			if(rule[rule.size()-1]==L'/')
				rule=rule.substr(0,rule.size()-1);
			return rule;
		}
		void AddMatchingRule(wstring rule){
			MatchingRules.push_back(rule);
		}
		void AddNotMatchingRule(wstring rule){
			NotMatchingRules.push_back(rule);
		}
	public:
		void AddRule(wstring rule){
			if(rule[0]=='!')
				AddNotMatchingRule(PreprocessRule(rule.substr(1)));
			else
				AddMatchingRule(PreprocessRule(rule));
		}
		bool IsMatch(wstring s){
			if constexpr(default_as_all_match){
				if(IsMatchRules(s,MatchingRules))
					return true;
				if(IsMatchRules(s,NotMatchingRules))
					return false;
			}
			else{
				if(IsMatchRules(s,NotMatchingRules))
					return false;
				if(IsMatchRules(s,MatchingRules))
					return true;
			}
			return default_as_all_match;
		}
		bool operator()(wstring s){
			return IsMatch(s);
		}
		//ForDir
	private:
		static inline wstring base_path{};
		void ForDir_mapper(wstring path,function<void(wstring)>do_what){
			CDirEnum ef(path);
			CDirEnumEntry entry;
			while(ef.next(entry)){
				if(IsMatch(base_path+L"/"+entry.name)){
					if(entry.isdir){
						auto base_path_bak = base_path;
						base_path +=L"/"+ entry.name;
						ForDir_mapper(path + L"/" + entry.name,do_what);
						base_path = base_path_bak;
					}
					else{
						do_what(path+L"/"+entry.name);
					}
				}
			}
		}
		void ForDir_mapper(wstring path,function<void(wstring,wstring)>do_what){
			CDirEnum ef(path);
			CDirEnumEntry entry;
			while(ef.next(entry)){
				if(IsMatch(base_path+L"/"+entry.name)){
					if(entry.isdir){
						auto base_path_bak = base_path;
						base_path +=L"/"+ entry.name;
						ForDir_mapper(path + L"/" + entry.name,do_what);
						base_path = base_path_bak;
					}
					else{
						do_what(path+L"/"+entry.name,base_path+L"/"+entry.name);
					}
				}
			}
		}
	public:
		void ForDir(wstring Dir,function<void(wstring)>do_what){
			base_path = L"";
			ForDir_mapper(Dir,do_what);
		}
		void ForDir(wstring Dir, function<void(wstring,wstring)>do_what) {
			base_path = L"";
			ForDir_mapper(Dir,do_what);
		}
	};
	template class filepathMatcher_t<true>;
	template class filepathMatcher_t<false>;
}

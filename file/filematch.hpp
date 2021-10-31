#include <string>
#include <vector>
#include <functional>
namespace filepathMatcher_n{
	template<bool default_as_all_match=true>
	class filepathMatcher_t{
		typedef filepathMatcher_t<default_as_all_match> self_t;
		std::vector<std::wstring> MatchingRules;//*.ai
		std::vector<std::wstring> NotMatchingRules;//!*.inf

		std::wstring PreprocessRule(std::wstring rule);
		void AddMatchingRule(std::wstring rule);
		void AddNotMatchingRule(std::wstring rule);
	public:
		void clear(){
			MatchingRules.clear();
			NotMatchingRules.clear();
		}
		void AddRule(std::wstring rule);
		bool IsMatch(std::wstring s);
		bool operator()(std::wstring s){
			return IsMatch(s);
		}
		void reverse() {
			swap(MatchingRules, NotMatchingRules);
		}
		//ForDir
	private:
		static std::wstring base_path;
		void ForDir_mapper(std::wstring path, std::function<void(std::wstring)>do_what);
		void ForDir_mapper(std::wstring path, std::function<void(std::wstring, std::wstring)>do_what);
	public:
		void ForDir(std::wstring Dir, std::function<void(std::wstring)>do_what);
		void ForDir(std::wstring Dir, std::function<void(std::wstring, std::wstring)>do_what);
	};
	typedef filepathMatcher_t<true> DefaultAllMatchFilepathMatcher;
	typedef filepathMatcher_t<false> DefaultNoneMatchFilepathMatcher;
}

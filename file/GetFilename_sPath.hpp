#include <string>
inline std::wstring GetFilename_sPath(std::wstring a){
	return a.substr(0,a.find_last_of(L"/\\")+1);
}

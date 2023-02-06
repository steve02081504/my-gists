#include "TCHAR.h"
#include "pdh.h"
class CPUusage {
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	double value;
public:
	CPUusage();
	~CPUusage();
	double getCurrentValue();
	double operator()() {
		return getCurrentValue();
	}
};

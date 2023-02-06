#include "TCHAR.h"
#include "pdh.h"
#pragma comment(lib,"pdh.lib")
class CPUusage {
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	double value;
public:
	CPUusage() {
		PdhOpenQuery(NULL, NULL, &cpuQuery);
		// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
		PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
		PdhCollectQueryData(cpuQuery);
		value = getCurrentValue_base();
	}
	~CPUusage()
	{
		PdhCloseQuery(cpuQuery);
	}
private:
	double getCurrentValue_base() {
		PDH_FMT_COUNTERVALUE counterVal;

		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		return counterVal.doubleValue;
	}
public:
	double getCurrentValue() {
		value = (value + getCurrentValue_base()) / 2;
		return value;
	}
	double operator()() {
		return getCurrentValue();
	}
};

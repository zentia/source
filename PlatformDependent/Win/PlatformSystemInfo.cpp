#include "SourcePrefix.h"
#include <winerror.h>
#include <windef.h>

#include "PlatformSystemInfo.h"

#include <winioctl.h>
#include <ShlObj.h>


static const UInt64 kMegabyte = 1048576ULL;

int systeminfo::GetPhysicalMemoryMB()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(memInfo);
	memInfo.ullTotalPhys = 0;
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG mb = memInfo.ullTotalPhys / kMegabyte;
	return int(mb);
}

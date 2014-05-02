#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

extern "C"
{
	void printCurrentStackTrace();
	bool getErrnoToName(int32_t errNo, char* name);
}

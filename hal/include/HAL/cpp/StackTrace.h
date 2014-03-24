
#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

#ifndef HAL_STACKTRACE_H
#define HAL_STACKTRACE_H

extern "C" {
  void printCurrentStackTrace();
  bool getErrnoToName(int32_t errNo, char* name);
}
#endif

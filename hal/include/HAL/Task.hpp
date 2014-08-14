#pragma once

#include <stdint.h>
#include <pthread.h>

#ifndef _FUNCPTR_DEFINED
#define _FUNCPTR_DEFINED
#ifdef __cplusplus
typedef int (*FUNCPTR)(...);
/* ptr to function returning int */
#else
typedef int (*FUNCPTR) (); /* ptr to function returning int */
#endif /* __cplusplus */
#endif /* _FUNCPTR_DEFINED */

#ifndef _STATUS_DEFINED
#define _STATUS_DEFINED
typedef int STATUS;
#endif /* _STATUS_DEFINED */

#ifndef OK
#define OK 0
#endif /* OK */
#ifndef ERROR
#define ERROR (-1)
#endif /* ERROR */

#define NULL_TASK NULL
typedef pthread_t* TASK;

extern "C" {
  // Note: These constants used to be declared extern and were defined in
  // Task.cpp. This caused issues with the JNI bindings for java, and so the
  // instantiations were moved here.
  const int32_t HAL_taskLib_ILLEGAL_PRIORITY = 22; // 22 is EINVAL

  STATUS verifyTaskID(TASK task);
  STATUS setTaskPriority(TASK task, int priority); // valid priority [1..99]
  STATUS getTaskPriority(TASK task, int* priority);
}

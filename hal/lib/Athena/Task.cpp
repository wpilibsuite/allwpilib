#include "HAL/Task.hpp"

#ifndef OK
#define OK             0
#endif /* OK */
#ifndef ERROR
#define ERROR          (-1)
#endif /* ERROR */

#include <signal.h>

STATUS verifyTaskID(TASK task) {
  if (task != nullptr && pthread_kill(*task, 0) == 0) {
	return OK;
  } else {
	return ERROR;
  }
}

STATUS setTaskPriority(TASK task, int priority) {
  int policy = 0;
  struct sched_param param;

  if (verifyTaskID(task) == OK &&
      pthread_getschedparam(*task, &policy, &param) == 0) {
    param.sched_priority = priority;
    if (pthread_setschedparam(*task, SCHED_FIFO, &param) == 0) {
      return OK;
    }
    else {
      return ERROR;
    }
  }
  else {
    return ERROR;
  }
}

STATUS getTaskPriority(TASK task, int* priority) {
  int policy = 0;
  struct sched_param param;

  if (verifyTaskID(task) == OK &&
    pthread_getschedparam(*task, &policy, &param) == 0) {
    *priority = param.sched_priority;
    return OK;
  }
  else {
    return ERROR;
  }
}

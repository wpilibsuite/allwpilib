
#include "HAL/Task.hpp"

#include <stdio.h>
#include <signal.h>

const uint32_t VXWORKS_FP_TASK = 0x01000000;
const int32_t HAL_objLib_OBJ_ID_ERROR = -1; // TODO: update to relevant TaskIDError
const int32_t HAL_objLib_OBJ_DELETED = -1; // TODO: update to relevant TaskDeletedError
const int32_t HAL_taskLib_ILLEGAL_OPTIONS = -1; // TODO: update to relevant TaskOptionsError
const int32_t HAL_memLib_NOT_ENOUGH_MEMORY = -1; // TODO: update to relevant TaskMemoryError
const int32_t HAL_taskLib_ILLEGAL_PRIORITY = -1; // TODO: update to relevant TaskPriorityError

struct TaskArgs {
	FUNCPTR fun;
	char* name;
	pthread_t** task;
	uint32_t arg0, arg1, arg2, arg3, arg4,
	         arg5, arg6, arg7, arg8, arg9;
};

void* startRoutine(void* data) {
	TaskArgs* args = (TaskArgs*) data;
	printf("[HAL] Starting task %s...\n", args->name);
	int val = args->fun(args->arg0, args->arg1, args->arg2, args->arg3, args->arg4,
			         args->arg5, args->arg6, args->arg7, args->arg8, args->arg9);
	printf("[HAL] Exited task %s with code %i\n", args->name, val);
	*args->task = NULL;
	int* ret = new int(); *ret = val;
	return ret;
}

TASK spawnTask(char * name, int priority, int options, int stackSize, 
		       FUNCPTR entryPt, uint32_t arg0, uint32_t arg1, uint32_t arg2,
		       uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6,
		       uint32_t arg7, uint32_t arg8, uint32_t arg9) {
  printf("[HAL] Spawning task %s...\n", name);
  pthread_t* task = new pthread_t;
  pthread_attr_t* attr = new pthread_attr_t;
  pthread_attr_init(attr);
  TaskArgs* args = new TaskArgs();
  args->fun = entryPt;
  args->name = name;
  args->task = new pthread_t*;
  *args->task = task;
  args->arg0 = arg0; args->arg1 = arg1; args->arg2 = arg2; args->arg3 = arg3; args->arg4 = arg4;
  args->arg5 = arg5; args->arg6 = arg6; args->arg7 = arg7; args->arg8 = arg8; args->arg9 = arg9;
  if (pthread_create(task, attr, startRoutine, args) == 0) {
	  printf("[HAL] Success\n");
	  pthread_detach(*task);
  } else {
	  printf("[HAL] Failure\n");
	  task = NULL;
  }
  pthread_attr_destroy(attr);
  return task;
}

STATUS restartTask(TASK task) {
  return ERROR; // TODO: implement;
}

STATUS deleteTask(TASK task) {
  return ERROR; // TODO: implement
}

STATUS isTaskReady(TASK task) {
  return ERROR; // TODO: implement
}

STATUS isTaskSuspended(TASK task) {
  return ERROR; // TODO: implement
}

STATUS suspendTask(TASK task) {
  return ERROR; // TODO: implement
}

STATUS resumeTask(TASK task) {
  return ERROR; // TODO: implement
}

STATUS verifyTaskID(TASK task) {
  if (task != NULL && pthread_kill(*task, 0) == 0) {
	return OK;
  } else {
	return ERROR;
  }
}

STATUS setTaskPriority(TASK task, int priority) {
  return ERROR; // TODO: implement
}

STATUS getTaskPriority(TASK task, int* priority) {
  return ERROR; // TODO: implement
}

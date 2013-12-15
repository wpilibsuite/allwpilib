
#include "HAL/Task.h"

#include "HAL/HAL.h"
#include "ChipObject.h"
#include <taskLib.h>
#include <usrLib.h>

const uint32_t VXWORKS_FP_TASK = VX_FP_TASK;
const int32_t HAL_objLib_OBJ_ID_ERROR = S_objLib_OBJ_ID_ERROR;
const int32_t HAL_objLib_OBJ_DELETED = S_objLib_OBJ_DELETED;
const int32_t HAL_taskLib_ILLEGAL_OPTIONS = S_taskLib_ILLEGAL_OPTIONS;
const int32_t HAL_memLib_NOT_ENOUGH_MEMORY = S_memLib_NOT_ENOUGH_MEMORY;
const int32_t HAL_taskLib_ILLEGAL_PRIORITY = S_taskLib_ILLEGAL_PRIORITY;

TASK spawnTask(char * name, int priority, int options, int stackSize, 
		       FUNCPTR entryPt, uint32_t arg0, uint32_t arg1, uint32_t arg2,
		       uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6,
		       uint32_t arg7, uint32_t arg8, uint32_t arg9) {
	return taskSpawn(name,
					 priority,
					 options,							// options
					 stackSize,						// stack size
					 entryPt,							// function to start
					 arg0, arg1, arg2, arg3, arg4,	// parameter 1 - pointer to this class
					 arg5, arg6, arg7, arg8, arg9);// additional unused parameters
  
}

STATUS restartTask(TASK task) {
  return taskRestart(task);
}

STATUS deleteTask(TASK task) {
  return taskDelete(task);
}

STATUS isTaskReady(TASK task) {
  return taskIsReady(task);
}

STATUS isTaskSuspended(TASK task) {
  return taskIsSuspended(task);
}

STATUS suspendTask(TASK task) {
  return taskSuspend(task);
}

STATUS resumeTask(TASK task) {
  return taskResume(task);
}

STATUS verifyTaskID(TASK task) {
  return taskIdVerify(task);
}

STATUS setTaskPriority(TASK task, int priority) {
  return taskPrioritySet(task, priority);
}

STATUS getTaskPriority(TASK task, int* priority) {
  return taskPriorityGet(task, priority);
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Semaphore.h"

#include "Log.h"

// set the logging level
TLogLevel semaphoreLogLevel = logDEBUG;

#define SEMAPHORE_LOG(level)     \
  if (level > semaphoreLogLevel) \
    ;                            \
  else                           \
  Log().Get(level)

extern "C" {

MUTEX_ID HAL_InitializeMutexNormal() { return new priority_mutex; }

void HAL_DeleteMutex(MUTEX_ID sem) { delete sem; }

/**
 * Lock the mutex, blocking until it's available.
 */
void HAL_TakeMutex(MUTEX_ID mutex) { mutex->lock(); }

/**
 * Attempt to lock the mutex.
 * @return true if succeeded in locking the mutex, false otherwise.
 */
bool HAL_TryTakeMutex(MUTEX_ID mutex) { return mutex->try_lock(); }

/**
 * Unlock the mutex.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
void HAL_GiveMutex(MUTEX_ID mutex) { mutex->unlock(); }

MULTIWAIT_ID HAL_InitializeMultiWait() {
  return new priority_condition_variable;
}

void HAL_DeleteMultiWait(MULTIWAIT_ID cond) { delete cond; }

void HAL_TakeMultiWait(MULTIWAIT_ID cond, MUTEX_ID m) {
  std::unique_lock<priority_mutex> lock(*m);
  cond->wait(lock);
}

void HAL_GiveMultiWait(MULTIWAIT_ID cond) { cond->notify_all(); }

}  // extern "C"

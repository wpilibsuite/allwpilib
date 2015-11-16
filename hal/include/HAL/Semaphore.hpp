#pragma once

#include "cpp/priority_condition_variable.h"
#include "cpp/priority_mutex.h"

typedef priority_mutex* MUTEX_ID;
typedef priority_condition_variable* MULTIWAIT_ID;
typedef priority_condition_variable::native_handle_type NATIVE_MULTIWAIT_ID;

extern "C" {
  MUTEX_ID initializeMutexNormal();
  void deleteMutex(MUTEX_ID sem);
  void takeMutex(MUTEX_ID sem);
  bool tryTakeMutex(MUTEX_ID sem);
  void giveMutex(MUTEX_ID sem);

  MULTIWAIT_ID initializeMultiWait();
  void deleteMultiWait(MULTIWAIT_ID sem);
  void takeMultiWait(MULTIWAIT_ID sem, MUTEX_ID m);
  void giveMultiWait(MULTIWAIT_ID sem);
}

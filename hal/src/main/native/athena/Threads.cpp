// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Threads.h"

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <system_error>

#include <fmt/format.h>

#include "hal/Errors.h"

namespace {
class UidSetter {
 public:
  explicit UidSetter(uid_t uid) {
    m_uid = geteuid();
    if (uid == 0 && setuid(uid) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              fmt::format("setuid({}) failed", uid));
    } else if (uid != 0 && seteuid(uid) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              fmt::format("seteuid({}) failed", uid));
    }
  }

  ~UidSetter() noexcept(false) {
    uid_t currentUid = geteuid();
    if (currentUid != m_uid) {
      if (seteuid(m_uid) == -1) {
        throw std::system_error(errno, std::generic_category(),
                                fmt::format("seteuid({}) failed", m_uid));
      }
    }
  }

 private:
  uid_t m_uid;
};
}  // namespace

namespace hal::init {
void InitializeThreads() {}
}  // namespace hal::init

extern "C" {

int32_t HAL_GetThreadPriority(NativeThreadHandle handle, HAL_Bool* isRealTime,
                              int32_t* status) {
  sched_param sch;
  int policy;
  int success = pthread_getschedparam(
      *reinterpret_cast<const pthread_t*>(handle), &policy, &sch);
  if (success == 0) {
    *status = 0;
  } else {
    *status = HAL_THREAD_PRIORITY_ERROR;
    return -1;
  }
  if (policy == SCHED_FIFO || policy == SCHED_RR) {
    *isRealTime = true;
    return sch.sched_priority;
  } else {
    *isRealTime = false;
    // 0 is the only supported priority for non-real-time
    return 0;
  }
}

int32_t HAL_GetCurrentThreadPriority(HAL_Bool* isRealTime, int32_t* status) {
  auto thread = pthread_self();
  return HAL_GetThreadPriority(&thread, isRealTime, status);
}

HAL_Bool HAL_SetThreadPriority(NativeThreadHandle handle, HAL_Bool realTime,
                               int32_t priority, int32_t* status) {
  if (handle == nullptr) {
    *status = NULL_PARAMETER;
    return false;
  }

  int scheduler = realTime ? SCHED_FIFO : SCHED_OTHER;
  if (realTime) {
    // We don't support setting priorities for non RT threads
    // so we don't need to check for proper range
    if (priority < sched_get_priority_min(scheduler) ||
        priority > sched_get_priority_max(scheduler)) {
      *status = HAL_THREAD_PRIORITY_RANGE_ERROR;
      return false;
    }
  }

  sched_param sch;
  int policy;
  pthread_getschedparam(*reinterpret_cast<const pthread_t*>(handle), &policy,
                        &sch);
  if (scheduler == SCHED_FIFO || scheduler == SCHED_RR) {
    sch.sched_priority = priority;
  } else {
    // Only need to set 0 priority for non RT thread
    sch.sched_priority = 0;
  }

  try {
    UidSetter uidSetter{0};

    if (pthread_setschedparam(*reinterpret_cast<const pthread_t*>(handle),
                              scheduler, &sch)) {
      *status = HAL_THREAD_PRIORITY_ERROR;
      return false;
    } else {
      *status = 0;
      return true;
    }
  } catch (const std::system_error& e) {
    *status = HAL_SETUID_ERROR;
    return false;
  }
}

HAL_Bool HAL_SetCurrentThreadPriority(HAL_Bool realTime, int32_t priority,
                                      int32_t* status) {
  auto thread = pthread_self();
  return HAL_SetThreadPriority(&thread, realTime, priority, status);
}

}  // extern "C"

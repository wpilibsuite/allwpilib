/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Notifier.h"

// For std::atexit()
#include <cstdlib>

#include <atomic>
#include <memory>
#include <mutex>

#include "HAL/ChipObject.h"
#include "HAL/Errors.h"
#include "HAL/HAL.h"
#include "HAL/cpp/make_unique.h"
#include "HAL/cpp/priority_mutex.h"
#include "HAL/handles/UnlimitedHandleResource.h"
#include "support/SafeThread.h"

using namespace hal;

static const int32_t kTimerInterruptNumber = 28;

static hal::priority_mutex notifierInterruptMutex;
static priority_recursive_mutex notifierMutex;
static std::unique_ptr<tAlarm> notifierAlarm;
static std::unique_ptr<tInterruptManager> notifierManager;
static uint64_t closestTrigger = UINT64_MAX;

namespace {
struct Notifier {
  std::shared_ptr<Notifier> prev, next;
  void* param;
  HAL_NotifierProcessFunction process;
  uint64_t triggerTime = UINT64_MAX;
  HAL_NotifierHandle handle;
  bool threaded;
};

// Safe thread to allow callbacks to run on their own thread
class NotifierThread : public wpi::SafeThread {
 public:
  void Main() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_active) {
      m_cond.wait(lock, [&] { return !m_active || m_notify; });
      if (!m_active) break;
      m_notify = false;
      uint64_t currentTime = m_currentTime;
      HAL_NotifierHandle handle = m_handle;
      HAL_NotifierProcessFunction process = m_process;
      lock.unlock();  // don't hold mutex during callback execution
      process(currentTime, handle);
      lock.lock();
    }
  }

  bool m_notify = false;
  HAL_NotifierHandle m_handle = HAL_kInvalidHandle;
  HAL_NotifierProcessFunction m_process;
  uint64_t m_currentTime;
};

class NotifierThreadOwner : public wpi::SafeThreadOwner<NotifierThread> {
 public:
  void SetFunc(HAL_NotifierProcessFunction process, void* param) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_process = process;
    m_param = param;
  }

  void Notify(uint64_t currentTime, HAL_NotifierHandle handle) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_currentTime = currentTime;
    thr->m_handle = handle;
    thr->m_notify = true;
    thr->m_cond.notify_one();
  }

  void* m_param;
};
}  // namespace

static std::shared_ptr<Notifier> notifiers;
static std::atomic_flag notifierAtexitRegistered = ATOMIC_FLAG_INIT;
static std::atomic_int notifierRefCount{0};

using namespace hal;

static UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                               HAL_HandleEnum::Notifier>
    notifierHandles;

// internal version of updateAlarm used during the alarmCallback when we know
// that the pointer is a valid pointer.
void updateNotifierAlarmInternal(std::shared_ptr<Notifier> notifierPointer,
                                 uint64_t triggerTime, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);

  auto notifier = notifierPointer;
  // no need for a null check, as this must always be a valid pointer.
  notifier->triggerTime = triggerTime;
  bool wasActive = (closestTrigger != UINT64_MAX);

  if (!notifierInterruptMutex.try_lock() || notifierRefCount == 0 ||
      !notifierAlarm)
    return;

  // Update alarm time if closer than current.
  if (triggerTime < closestTrigger) {
    closestTrigger = triggerTime;
    // Simply truncate the hardware trigger time to 32-bit.
    notifierAlarm->writeTriggerTime(static_cast<uint32_t>(triggerTime), status);
  }
  // Enable the alarm.  The hardware disables itself after each alarm.
  if (!wasActive) notifierAlarm->writeEnable(true, status);

  notifierInterruptMutex.unlock();
}

static void alarmCallback(uint32_t, void*) {
  std::unique_lock<priority_recursive_mutex> sync(notifierMutex);

  int32_t status = 0;
  uint64_t currentTime = 0;

  // the hardware disables itself after each alarm
  closestTrigger = UINT64_MAX;

  // process all notifiers
  std::shared_ptr<Notifier> notifier = notifiers;
  while (notifier) {
    if (notifier->triggerTime != UINT64_MAX) {
      if (currentTime == 0) currentTime = HAL_GetFPGATime(&status);
      if (notifier->triggerTime < currentTime) {
        notifier->triggerTime = UINT64_MAX;
        auto process = notifier->process;
        auto handle = notifier->handle;
        sync.unlock();
        process(currentTime, handle);
        sync.lock();
      } else if (notifier->triggerTime < closestTrigger) {
        updateNotifierAlarmInternal(notifier, notifier->triggerTime, &status);
      }
    }
    notifier = notifier->next;
  }
}

static void cleanupNotifierAtExit() {
  notifierAlarm = nullptr;
  notifierManager = nullptr;
}

static void threadedNotifierHandler(uint64_t currentTimeInt,
                                    HAL_NotifierHandle handle) {
  // Grab notifier and get handler param
  auto notifier = notifierHandles.Get(handle);
  if (!notifier) return;
  auto notifierPointer = notifier->param;
  if (notifierPointer == nullptr) return;
  NotifierThreadOwner* owner =
      static_cast<NotifierThreadOwner*>(notifierPointer);
  owner->Notify(currentTimeInt, handle);
}

extern "C" {

HAL_NotifierHandle HAL_InitializeNotifier(HAL_NotifierProcessFunction process,
                                          void* param, int32_t* status) {
  if (!process) {
    *status = NULL_PARAMETER;
    return 0;
  }
  if (!notifierAtexitRegistered.test_and_set())
    std::atexit(cleanupNotifierAtExit);
  if (notifierRefCount.fetch_add(1) == 0) {
    std::lock_guard<hal::priority_mutex> sync(notifierInterruptMutex);
    // create manager and alarm if not already created
    if (!notifierManager) {
      notifierManager = std::make_unique<tInterruptManager>(
          1 << kTimerInterruptNumber, false, status);
      notifierManager->registerHandler(alarmCallback, nullptr, status);
      notifierManager->enable(status);
    }
    if (!notifierAlarm) notifierAlarm.reset(tAlarm::create(status));
  }

  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
  std::shared_ptr<Notifier> notifier = std::make_shared<Notifier>();
  HAL_NotifierHandle handle = notifierHandles.Allocate(notifier);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  // create notifier structure and add to list
  notifier->next = notifiers;
  if (notifier->next) notifier->next->prev = notifier;
  notifier->param = param;
  notifier->process = process;
  notifier->handle = handle;
  notifier->threaded = false;
  notifiers = notifier;
  return handle;
}

HAL_NotifierHandle HAL_InitializeNotifierThreaded(
    HAL_NotifierProcessFunction process, void* param, int32_t* status) {
  NotifierThreadOwner* notify = new NotifierThreadOwner;
  notify->Start();
  notify->SetFunc(process, param);

  auto notifierHandle =
      HAL_InitializeNotifier(threadedNotifierHandler, notify, status);

  if (notifierHandle == HAL_kInvalidHandle || *status != 0) {
    delete notify;
    return HAL_kInvalidHandle;
  }

  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) {
    return HAL_kInvalidHandle;
  }
  notifier->threaded = true;

  return notifierHandle;
}

void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  {
    std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
    auto notifier = notifierHandles.Get(notifierHandle);
    if (!notifier) return;

    // remove from list
    if (notifier->prev) notifier->prev->next = notifier->next;
    if (notifier->next) notifier->next->prev = notifier->prev;
    if (notifiers == notifier) notifiers = notifier->next;
    notifierHandles.Free(notifierHandle);

    if (notifier->threaded) {
      NotifierThreadOwner* owner =
          static_cast<NotifierThreadOwner*>(notifier->param);
      delete owner;
    }
  }

  if (notifierRefCount.fetch_sub(1) == 1) {
    std::lock_guard<hal::priority_mutex> sync(notifierInterruptMutex);
    // if this was the last notifier, clean up alarm and manager
    if (notifierAlarm) {
      notifierAlarm->writeEnable(false, status);
      notifierAlarm = nullptr;
    }
    if (notifierManager) {
      notifierManager->disable(status);
      notifierManager = nullptr;
    }
    closestTrigger = UINT64_MAX;
  }
}

void* HAL_GetNotifierParam(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return nullptr;
  if (notifier->threaded) {
    // If threaded, return thread param rather then notifier param
    NotifierThreadOwner* owner =
        static_cast<NotifierThreadOwner*>(notifier->param);
    return owner->m_param;
  }
  return notifier->param;
}

void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);

  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return;
  updateNotifierAlarmInternal(notifier, triggerTime, status);
}

void HAL_StopNotifierAlarm(HAL_NotifierHandle notifierHandle, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return;
  notifier->triggerTime = UINT64_MAX;
}

}  // extern "C"

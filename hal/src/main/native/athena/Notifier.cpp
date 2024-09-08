// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Notifier.h"

#include <atomic>
#include <cstdlib>  // For std::atexit()
#include <memory>
#include <thread>

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/print.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/HAL.h"
#include "hal/Threads.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "hal/roborio/InterruptManager.h"

using namespace hal;

static constexpr int32_t kTimerInterruptNumber = 28;

static wpi::mutex notifierMutex;
static std::unique_ptr<tAlarm> notifierAlarm;
static std::thread notifierThread;
static HAL_Bool notifierThreadRealTime = false;
static int32_t notifierThreadPriority = 0;
static uint64_t closestTrigger{UINT64_MAX};

namespace {

struct Notifier {
  uint64_t triggerTime = UINT64_MAX;
  uint64_t triggeredTime = UINT64_MAX;
  bool active = true;
  wpi::mutex mutex;
  wpi::condition_variable cond;
};

}  // namespace

static std::atomic_flag notifierAtexitRegistered{ATOMIC_FLAG_INIT};
static std::atomic_int notifierRefCount{0};
static std::atomic_bool notifierRunning{false};

using namespace hal;

class NotifierHandleContainer
    : public UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                                     HAL_HandleEnum::Notifier> {
 public:
  ~NotifierHandleContainer() {
    ForEach([](HAL_NotifierHandle handle, Notifier* notifier) {
      {
        std::scoped_lock lock(notifier->mutex);
        notifier->triggerTime = UINT64_MAX;
        notifier->triggeredTime = 0;
        notifier->active = false;
      }
      notifier->cond.notify_all();  // wake up any waiting threads
    });
  }
};

static NotifierHandleContainer* notifierHandles;

static void alarmCallback() {
  std::scoped_lock lock(notifierMutex);
  int32_t status = 0;
  uint64_t currentTime = 0;

  // the hardware disables itself after each alarm
  closestTrigger = UINT64_MAX;

  // process all notifiers
  notifierHandles->ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    if (notifier->triggerTime == UINT64_MAX) {
      return;
    }
    if (currentTime == 0) {
      currentTime = HAL_GetFPGATime(&status);
    }
    std::unique_lock lock(notifier->mutex);
    if (notifier->triggerTime < currentTime) {
      notifier->triggerTime = UINT64_MAX;
      notifier->triggeredTime = currentTime;
      lock.unlock();
      notifier->cond.notify_all();
    } else if (notifier->triggerTime < closestTrigger) {
      closestTrigger = notifier->triggerTime;
    }
  });

  if (notifierAlarm && closestTrigger != UINT64_MAX) {
    // Simply truncate the hardware trigger time to 32-bit.
    notifierAlarm->writeTriggerTime(static_cast<uint32_t>(closestTrigger),
                                    &status);
    // Enable the alarm.  The hardware disables itself after each alarm.
    notifierAlarm->writeEnable(true, &status);
  }
}

static void notifierThreadMain() {
  InterruptManager& manager = InterruptManager::GetInstance();
  NiFpga_IrqContext context = manager.GetContext();
  uint32_t mask = 1 << kTimerInterruptNumber;
  int32_t status = 0;

  while (notifierRunning) {
    status = 0;
    auto triggeredMask =
        manager.WaitForInterrupt(context, mask, false, 10000, &status);
    if (!notifierRunning) {
      break;
    }
    if (triggeredMask == 0) {
      continue;
    }
    alarmCallback();
  }
}

static void cleanupNotifierAtExit() {
  int32_t status = 0;
  if (notifierAlarm) {
    notifierAlarm->writeEnable(false, &status);
  }
  notifierAlarm = nullptr;
  notifierRunning = false;
  hal::ReleaseFPGAInterrupt(kTimerInterruptNumber);
  if (notifierThread.joinable()) {
    notifierThread.join();
  }
}

namespace hal::init {
void InitializeNotifier() {
  static NotifierHandleContainer nH;
  notifierHandles = &nH;
}
}  // namespace hal::init

extern "C" {

HAL_NotifierHandle HAL_InitializeNotifier(int32_t* status) {
  hal::init::CheckInit();
  if (!notifierAtexitRegistered.test_and_set()) {
    std::atexit(cleanupNotifierAtExit);
  }

  if (notifierRefCount.fetch_add(1) == 0) {
    std::scoped_lock lock(notifierMutex);
    notifierRunning = true;
    notifierThread = std::thread(notifierThreadMain);

    auto native = notifierThread.native_handle();
    HAL_SetThreadPriority(&native, notifierThreadRealTime,
                          notifierThreadPriority, status);
    if (*status == HAL_THREAD_PRIORITY_ERROR) {
      *status = 0;
      wpi::print("{}: HAL Notifier thread\n",
                 HAL_THREAD_PRIORITY_ERROR_MESSAGE);
    }
    if (*status == HAL_THREAD_PRIORITY_RANGE_ERROR) {
      *status = 0;
      wpi::print("{}: HAL Notifier thread\n",
                 HAL_THREAD_PRIORITY_RANGE_ERROR_MESSAGE);
    }

    notifierAlarm.reset(tAlarm::create(status));
  }

  std::shared_ptr<Notifier> notifier = std::make_shared<Notifier>();
  HAL_NotifierHandle handle = notifierHandles->Allocate(notifier);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  return handle;
}

HAL_Bool HAL_SetNotifierThreadPriority(HAL_Bool realTime, int32_t priority,
                                       int32_t* status) {
  std::scoped_lock lock(notifierMutex);
  notifierThreadRealTime = realTime;
  notifierThreadPriority = priority;
  if (notifierThread.joinable()) {
    auto native = notifierThread.native_handle();
    return HAL_SetThreadPriority(&native, realTime, priority, status);
  } else {
    return true;
  }
}

void HAL_SetNotifierName(HAL_NotifierHandle notifierHandle, const char* name,
                         int32_t* status) {}

void HAL_StopNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }

  {
    std::scoped_lock lock(notifier->mutex);
    notifier->triggerTime = UINT64_MAX;
    notifier->triggeredTime = 0;
    notifier->active = false;
  }
  notifier->cond.notify_all();  // wake up any waiting threads
}

void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle) {
  auto notifier = notifierHandles->Free(notifierHandle);
  if (!notifier) {
    return;
  }

  // Just in case HAL_StopNotifier() wasn't called...
  {
    std::scoped_lock lock(notifier->mutex);
    notifier->triggerTime = UINT64_MAX;
    notifier->triggeredTime = 0;
    notifier->active = false;
  }
  notifier->cond.notify_all();

  if (notifierRefCount.fetch_sub(1) == 1) {
    // if this was the last notifier, clean up alarm and thread
    // the notifier can call back into our callback, so don't hold the lock
    // here (the atomic fetch_sub will prevent multiple parallel entries
    // into this function)
    int32_t status = 0;
    if (notifierAlarm) {
      notifierAlarm->writeEnable(false, &status);
    }
    notifierRunning = false;
    hal::ReleaseFPGAInterrupt(kTimerInterruptNumber);
    if (notifierThread.joinable()) {
      notifierThread.join();
    }

    std::scoped_lock lock(notifierMutex);
    notifierAlarm = nullptr;
    closestTrigger = UINT64_MAX;
  }
}

void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }

  {
    std::scoped_lock lock(notifier->mutex);
    notifier->triggerTime = triggerTime;
    notifier->triggeredTime = UINT64_MAX;
  }

  std::scoped_lock lock(notifierMutex);
  // Update alarm time if closer than current.
  if (triggerTime < closestTrigger) {
    bool wasActive = (closestTrigger != UINT64_MAX);
    closestTrigger = triggerTime;
    // Simply truncate the hardware trigger time to 32-bit.
    notifierAlarm->writeTriggerTime(static_cast<uint32_t>(closestTrigger),
                                    status);
    // Enable the alarm.
    if (!wasActive) {
      notifierAlarm->writeEnable(true, status);
    }
  }
}

void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }

  {
    std::scoped_lock lock(notifier->mutex);
    notifier->triggerTime = UINT64_MAX;
  }
}

uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return 0;
  }
  std::unique_lock lock(notifier->mutex);
  notifier->cond.wait(lock, [&] {
    return !notifier->active || notifier->triggeredTime != UINT64_MAX;
  });
  return notifier->active ? notifier->triggeredTime : 0;
}

}  // extern "C"

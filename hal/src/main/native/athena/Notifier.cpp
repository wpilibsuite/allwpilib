/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Notifier.h"

#include <atomic>
#include <cstdlib>  // For std::atexit()
#include <memory>

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/HAL.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

static constexpr int32_t kTimerInterruptNumber = 28;

static wpi::mutex notifierMutex;
static std::unique_ptr<tAlarm> notifierAlarm;
static std::unique_ptr<tInterruptManager> notifierManager;
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

using namespace hal;

class NotifierHandleContainer
    : public UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                                     HAL_HandleEnum::Notifier> {
 public:
  ~NotifierHandleContainer() {
    ForEach([](HAL_NotifierHandle handle, Notifier* notifier) {
      {
        std::lock_guard<wpi::mutex> lock(notifier->mutex);
        notifier->triggerTime = UINT64_MAX;
        notifier->triggeredTime = 0;
        notifier->active = false;
      }
      notifier->cond.notify_all();  // wake up any waiting threads
    });
  }
};

static NotifierHandleContainer* notifierHandles;

static void alarmCallback(uint32_t, void*) {
  std::lock_guard<wpi::mutex> lock(notifierMutex);
  int32_t status = 0;
  uint64_t currentTime = 0;

  // the hardware disables itself after each alarm
  closestTrigger = UINT64_MAX;

  // process all notifiers
  notifierHandles->ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    if (notifier->triggerTime == UINT64_MAX) return;
    if (currentTime == 0) currentTime = HAL_GetFPGATime(&status);
    std::unique_lock<wpi::mutex> lock(notifier->mutex);
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

static void cleanupNotifierAtExit() {
  notifierAlarm = nullptr;
  notifierManager = nullptr;
}

namespace hal {
namespace init {
void InitializeNotifier() {
  static NotifierHandleContainer nH;
  notifierHandles = &nH;
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_NotifierHandle HAL_InitializeNotifier(int32_t* status) {
  hal::init::CheckInit();
  if (!notifierAtexitRegistered.test_and_set())
    std::atexit(cleanupNotifierAtExit);

  if (notifierRefCount.fetch_add(1) == 0) {
    std::lock_guard<wpi::mutex> lock(notifierMutex);
    // create manager and alarm if not already created
    if (!notifierManager) {
      notifierManager = std::make_unique<tInterruptManager>(
          1 << kTimerInterruptNumber, false, status);
      notifierManager->registerHandler(alarmCallback, nullptr, status);
      notifierManager->enable(status);
    }
    if (!notifierAlarm) notifierAlarm.reset(tAlarm::create(status));
  }

  std::shared_ptr<Notifier> notifier = std::make_shared<Notifier>();
  HAL_NotifierHandle handle = notifierHandles->Allocate(notifier);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  return handle;
}

void HAL_StopNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return;

  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->triggerTime = UINT64_MAX;
    notifier->triggeredTime = 0;
    notifier->active = false;
  }
  notifier->cond.notify_all();  // wake up any waiting threads
}

void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles->Free(notifierHandle);
  if (!notifier) return;

  // Just in case HAL_StopNotifier() wasn't called...
  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->triggerTime = UINT64_MAX;
    notifier->triggeredTime = 0;
    notifier->active = false;
  }
  notifier->cond.notify_all();

  if (notifierRefCount.fetch_sub(1) == 1) {
    // if this was the last notifier, clean up alarm and manager
    // the notifier can call back into our callback, so don't hold the lock
    // here (the atomic fetch_sub will prevent multiple parallel entries
    // into this function)

    // Cleaning up the manager takes up to a second to complete, so don't do
    // that here. Fix it more permanently in 2019...

    // if (notifierAlarm) notifierAlarm->writeEnable(false, status);
    // if (notifierManager) notifierManager->disable(status);

    // std::lock_guard<wpi::mutex> lock(notifierMutex);
    // notifierAlarm = nullptr;
    // notifierManager = nullptr;
    // closestTrigger = UINT64_MAX;
  }
}

void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return;

  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->triggerTime = triggerTime;
    notifier->triggeredTime = UINT64_MAX;
  }

  std::lock_guard<wpi::mutex> lock(notifierMutex);
  // Update alarm time if closer than current.
  if (triggerTime < closestTrigger) {
    bool wasActive = (closestTrigger != UINT64_MAX);
    closestTrigger = triggerTime;
    // Simply truncate the hardware trigger time to 32-bit.
    notifierAlarm->writeTriggerTime(static_cast<uint32_t>(closestTrigger),
                                    status);
    // Enable the alarm.
    if (!wasActive) notifierAlarm->writeEnable(true, status);
  }
}

void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return;

  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->triggerTime = UINT64_MAX;
  }
}

uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return 0;
  std::unique_lock<wpi::mutex> lock(notifier->mutex);
  notifier->cond.wait(lock, [&] {
    return !notifier->active || notifier->triggeredTime != UINT64_MAX;
  });
  return notifier->active ? notifier->triggeredTime : 0;
}

}  // extern "C"

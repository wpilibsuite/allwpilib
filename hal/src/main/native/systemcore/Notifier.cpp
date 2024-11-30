// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Notifier.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "hal/simulation/NotifierData.h"

namespace {
struct Notifier {
  std::string name;
  uint64_t waitTime = UINT64_MAX;
  bool active = true;
  bool waitTimeValid = false;    // True if waitTime is set and in the future
  bool waitingForAlarm = false;  // True if in HAL_WaitForNotifierAlarm()
  uint64_t waitCount = 0;        // Counts calls to HAL_WaitForNotifierAlarm()
  wpi::mutex mutex;
  wpi::condition_variable cond;
};
}  // namespace

using namespace hal;

static wpi::mutex notifiersWaiterMutex;
static wpi::condition_variable notifiersWaiterCond;

class NotifierHandleContainer
    : public UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                                     HAL_HandleEnum::Notifier> {
 public:
  ~NotifierHandleContainer() {
    ForEach([](HAL_NotifierHandle handle, Notifier* notifier) {
      {
        std::scoped_lock lock(notifier->mutex);
        notifier->active = false;
        notifier->waitTimeValid = false;
      }
      notifier->cond.notify_all();  // wake up any waiting threads
    });
    notifiersWaiterCond.notify_all();
  }
};

static NotifierHandleContainer* notifierHandles;
static std::atomic<bool> notifiersPaused{false};

namespace hal::init {
void InitializeNotifier() {
  static NotifierHandleContainer nH;
  notifierHandles = &nH;
}
}  // namespace hal::init

extern "C" {

HAL_NotifierHandle HAL_InitializeNotifier(int32_t* status) {
  hal::init::CheckInit();
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
  // TODO fix this
  return true;
}

void HAL_SetNotifierName(HAL_NotifierHandle notifierHandle, const char* name,
                         int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }
  std::scoped_lock lock(notifier->mutex);
  notifier->name = name;
}

void HAL_StopNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }

  {
    std::scoped_lock lock(notifier->mutex);
    notifier->active = false;
    notifier->waitTimeValid = false;
  }
  notifier->cond.notify_all();
}

void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle) {
  auto notifier = notifierHandles->Free(notifierHandle);
  if (!notifier) {
    return;
  }

  // Just in case HAL_StopNotifier() wasn't called...
  {
    std::scoped_lock lock(notifier->mutex);
    notifier->active = false;
    notifier->waitTimeValid = false;
  }
  notifier->cond.notify_all();
}

void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }

  {
    std::scoped_lock lock(notifier->mutex);
    notifier->waitTime = triggerTime;
    notifier->waitTimeValid = (triggerTime != UINT64_MAX);
  }

  // We wake up any waiters to change how long they're sleeping for
  notifier->cond.notify_all();
}

void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return;
  }

  {
    std::scoped_lock lock(notifier->mutex);
    notifier->waitTimeValid = false;
  }
}

uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) {
    return 0;
  }

  std::unique_lock ulock(notifiersWaiterMutex);
  std::unique_lock lock(notifier->mutex);
  notifier->waitingForAlarm = true;
  ++notifier->waitCount;
  ulock.unlock();
  notifiersWaiterCond.notify_all();
  while (notifier->active) {
    uint64_t curTime = HAL_GetFPGATime(status);
    if (notifier->waitTimeValid && curTime >= notifier->waitTime) {
      notifier->waitTimeValid = false;
      notifier->waitingForAlarm = false;
      return curTime;
    }

    double waitDuration;
    if (!notifier->waitTimeValid || notifiersPaused) {
      // If not running, wait 1000 seconds
      waitDuration = 1000.0;
    } else {
      waitDuration = (notifier->waitTime - curTime) * 1e-6;
    }

    notifier->cond.wait_for(lock, std::chrono::duration<double>(waitDuration));
  }
  notifier->waitingForAlarm = false;
  return 0;
}

}  // extern "C"

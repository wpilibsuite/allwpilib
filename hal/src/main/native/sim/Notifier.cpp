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
#include "NotifierInternal.h"
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

namespace hal {
namespace init {
void InitializeNotifier() {
  static NotifierHandleContainer nH;
  notifierHandles = &nH;
}
}  // namespace init

void PauseNotifiers() {
  notifiersPaused = true;
}

void ResumeNotifiers() {
  notifiersPaused = false;
  WakeupNotifiers();
}

void WakeupNotifiers() {
  notifierHandles->ForEach([](HAL_NotifierHandle handle, Notifier* notifier) {
    notifier->cond.notify_all();
  });
}

void WaitNotifiers() {
  std::unique_lock ulock(notifiersWaiterMutex);
  wpi::SmallVector<HAL_NotifierHandle, 8> waiters;

  // Wait for all Notifiers to hit HAL_WaitForNotifierAlarm()
  notifierHandles->ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    std::scoped_lock lock(notifier->mutex);
    if (notifier->active && !notifier->waitingForAlarm) {
      waiters.emplace_back(handle);
    }
  });
  for (;;) {
    int count = 0;
    int end = waiters.size();
    while (count < end) {
      auto& it = waiters[count];
      if (auto notifier = notifierHandles->Get(it)) {
        std::scoped_lock lock(notifier->mutex);
        if (notifier->active && !notifier->waitingForAlarm) {
          ++count;
          continue;
        }
      }
      // No longer need to wait for it, put at end so it can be erased
      std::swap(it, waiters[--end]);
    }
    if (count == 0) {
      break;
    }
    waiters.resize(count);
    notifiersWaiterCond.wait_for(ulock, std::chrono::duration<double>(1));
  }
}

void WakeupWaitNotifiers() {
  std::unique_lock ulock(notifiersWaiterMutex);
  int32_t status = 0;
  uint64_t curTime = HAL_GetFPGATime(&status);
  wpi::SmallVector<std::pair<HAL_NotifierHandle, uint64_t>, 8> waiters;

  // Wake up Notifiers that have expired timeouts
  notifierHandles->ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    std::scoped_lock lock(notifier->mutex);

    // Only wait for the Notifier if it has a valid timeout that's expired
    if (notifier->active && notifier->waitTimeValid &&
        curTime >= notifier->waitTime) {
      waiters.emplace_back(handle, notifier->waitCount);
      notifier->cond.notify_all();
    }
  });
  for (;;) {
    int count = 0;
    int end = waiters.size();
    while (count < end) {
      auto& it = waiters[count];
      if (auto notifier = notifierHandles->Get(it.first)) {
        std::scoped_lock lock(notifier->mutex);

        // waitCount is used here instead of waitingForAlarm because we want to
        // wait until HAL_WaitForNotifierAlarm() is exited, then reentered
        if (notifier->active && notifier->waitCount == it.second) {
          ++count;
          continue;
        }
      }
      // No longer need to wait for it, put at end so it can be erased
      it.swap(waiters[--end]);
    }
    if (count == 0) {
      break;
    }
    waiters.resize(count);
    notifiersWaiterCond.wait_for(ulock, std::chrono::duration<double>(1));
  }
}
}  // namespace hal

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

uint64_t HALSIM_GetNextNotifierTimeout(void) {
  uint64_t timeout = UINT64_MAX;
  notifierHandles->ForEach([&](HAL_NotifierHandle, Notifier* notifier) {
    std::scoped_lock lock(notifier->mutex);
    if (notifier->active && notifier->waitTimeValid &&
        timeout > notifier->waitTime) {
      timeout = notifier->waitTime;
    }
  });
  return timeout;
}

int32_t HALSIM_GetNumNotifiers(void) {
  int32_t count = 0;
  notifierHandles->ForEach([&](HAL_NotifierHandle, Notifier* notifier) {
    std::scoped_lock lock(notifier->mutex);
    if (notifier->active) {
      ++count;
    }
  });
  return count;
}

int32_t HALSIM_GetNotifierInfo(struct HALSIM_NotifierInfo* arr, int32_t size) {
  int32_t num = 0;
  notifierHandles->ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    std::scoped_lock lock(notifier->mutex);
    if (!notifier->active) {
      return;
    }
    if (num < size) {
      arr[num].handle = handle;
      if (notifier->name.empty()) {
        wpi::format_to_n_c_str(arr[num].name, sizeof(arr[num].name),
                               "Notifier{}",
                               static_cast<int>(getHandleIndex(handle)));
      } else {
        std::strncpy(arr[num].name, notifier->name.c_str(),
                     sizeof(arr[num].name) - 1);
        arr[num].name[sizeof(arr[num].name) - 1] = '\0';
      }
      arr[num].timeout = notifier->waitTime;
      arr[num].waitTimeValid = notifier->waitTimeValid;
    }
    ++num;
  });
  return num;
}

}  // extern "C"

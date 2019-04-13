/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Notifier.h"

#include <chrono>

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/timestamp.h>

#include "HALInitializer.h"
#include "hal/HAL.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/UnlimitedHandleResource.h"

namespace {
struct Notifier {
  uint64_t waitTime;
  bool updatedAlarm = false;
  bool active = true;
  bool running = false;
  wpi::mutex mutex;
  wpi::condition_variable cond;
};
}  // namespace

using namespace hal;

class NotifierHandleContainer
    : public UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                                     HAL_HandleEnum::Notifier> {
 public:
  ~NotifierHandleContainer() {
    ForEach([](HAL_NotifierHandle handle, Notifier* notifier) {
      {
        std::lock_guard<wpi::mutex> lock(notifier->mutex);
        notifier->active = false;
        notifier->running = false;
      }
      notifier->cond.notify_all();  // wake up any waiting threads
    });
  }
};

static NotifierHandleContainer* notifierHandles;

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
    notifier->active = false;
    notifier->running = false;
  }
  notifier->cond.notify_all();
}

void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles->Free(notifierHandle);
  if (!notifier) return;

  // Just in case HAL_StopNotifier() wasn't called...
  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->active = false;
    notifier->running = false;
  }
  notifier->cond.notify_all();
}

void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return;

  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->waitTime = triggerTime;
    notifier->running = true;
    notifier->updatedAlarm = true;
  }

  // We wake up any waiters to change how long they're sleeping for
  notifier->cond.notify_all();
}

void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return;

  {
    std::lock_guard<wpi::mutex> lock(notifier->mutex);
    notifier->running = false;
  }
}

uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status) {
  auto notifier = notifierHandles->Get(notifierHandle);
  if (!notifier) return 0;

  std::unique_lock<wpi::mutex> lock(notifier->mutex);
  while (notifier->active) {
    double waitTime;
    if (!notifier->running) {
      waitTime = (HAL_GetFPGATime(status) * 1e-6) + 1000.0;
      // If not running, wait 1000 seconds
    } else {
      waitTime = notifier->waitTime * 1e-6;
    }

    // Don't wait twice
    notifier->updatedAlarm = false;

    auto timeoutTime =
        hal::fpga_clock::epoch() + std::chrono::duration<double>(waitTime);
    notifier->cond.wait_until(lock, timeoutTime);
    if (notifier->updatedAlarm) {
      notifier->updatedAlarm = false;
      continue;
    }
    if (!notifier->running) continue;
    if (!notifier->active) break;
    notifier->running = false;
    return HAL_GetFPGATime(status);
  }
  return 0;
}

}  // extern "C"

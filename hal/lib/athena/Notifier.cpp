/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
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

#include "ChipObject.h"
#include "HAL/HAL.h"
#include "HAL/cpp/priority_mutex.h"
#include "handles/UnlimitedHandleResource.h"

static const uint32_t kTimerInterruptNumber = 28;

static priority_mutex notifierInterruptMutex;
static priority_recursive_mutex notifierMutex;
static tAlarm* notifierAlarm = nullptr;
static tInterruptManager* notifierManager = nullptr;
static uint64_t closestTrigger = UINT64_MAX;

namespace {
struct Notifier {
  std::shared_ptr<Notifier> prev, next;
  void* param;
  void (*process)(uint64_t, void*);
  uint64_t triggerTime = UINT64_MAX;
};
}
static std::shared_ptr<Notifier> notifiers = nullptr;
static std::atomic_flag notifierAtexitRegistered = ATOMIC_FLAG_INIT;
static std::atomic_int notifierRefCount{0};

using namespace hal;

static UnlimitedHandleResource<HalNotifierHandle, Notifier,
                               HalHandleEnum::Notifier>
    notifierHandles;

// internal version of updateAlarm used during the alarmCallback when we know
// that the pointer is a valid pointer.
void updateNotifierAlarmInternal(std::shared_ptr<Notifier> notifier_pointer,
                                 uint64_t triggerTime, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);

  auto notifier = notifier_pointer;
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
    notifierAlarm->writeTriggerTime((uint32_t)triggerTime, status);
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
      if (currentTime == 0) currentTime = getFPGATime(&status);
      if (notifier->triggerTime < currentTime) {
        notifier->triggerTime = UINT64_MAX;
        auto process = notifier->process;
        auto param = notifier->param;
        sync.unlock();
        process(currentTime, param);
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

extern "C" {

HalNotifierHandle initializeNotifier(void (*process)(uint64_t, void*),
                                     void* param, int32_t* status) {
  if (!process) {
    *status = NULL_PARAMETER;
    return 0;
  }
  if (!notifierAtexitRegistered.test_and_set())
    std::atexit(cleanupNotifierAtExit);
  if (notifierRefCount.fetch_add(1) == 0) {
    std::lock_guard<priority_mutex> sync(notifierInterruptMutex);
    // create manager and alarm if not already created
    if (!notifierManager) {
      notifierManager =
          new tInterruptManager(1 << kTimerInterruptNumber, false, status);
      notifierManager->registerHandler(alarmCallback, nullptr, status);
      notifierManager->enable(status);
    }
    if (!notifierAlarm) notifierAlarm = tAlarm::create(status);
  }

  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
  // create notifier structure and add to list
  std::shared_ptr<Notifier> notifier = std::make_shared<Notifier>();
  notifier->next = notifiers;
  if (notifier->next) notifier->next->prev = notifier;
  notifier->param = param;
  notifier->process = process;
  notifiers = notifier;
  return notifierHandles.Allocate(notifier);
}

void cleanNotifier(HalNotifierHandle notifier_handle, int32_t* status) {
  {
    std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
    auto notifier = notifierHandles.Get(notifier_handle);
    if (!notifier) return;

    // remove from list
    if (notifier->prev) notifier->prev->next = notifier->next;
    if (notifier->next) notifier->next->prev = notifier->prev;
    if (notifiers == notifier) notifiers = notifier->next;
    notifierHandles.Free(notifier_handle);
  }

  if (notifierRefCount.fetch_sub(1) == 1) {
    std::lock_guard<priority_mutex> sync(notifierInterruptMutex);
    // if this was the last notifier, clean up alarm and manager
    if (notifierAlarm) {
      notifierAlarm->writeEnable(false, status);
      delete notifierAlarm;
      notifierAlarm = nullptr;
    }
    if (notifierManager) {
      notifierManager->disable(status);
      delete notifierManager;
      notifierManager = nullptr;
    }
    closestTrigger = UINT64_MAX;
  }
}

void* getNotifierParam(HalNotifierHandle notifier_handle, int32_t* status) {
  auto notifier = notifierHandles.Get(notifier_handle);
  if (!notifier) return nullptr;
  return notifier->param;
}

void updateNotifierAlarm(HalNotifierHandle notifier_handle,
                         uint64_t triggerTime, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);

  auto notifier = notifierHandles.Get(notifier_handle);
  if (!notifier) return;
  updateNotifierAlarmInternal(notifier, triggerTime, status);
}

void stopNotifierAlarm(HalNotifierHandle notifier_handle, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
  auto notifier = notifierHandles.Get(notifier_handle);
  if (!notifier) return;
  notifier->triggerTime = UINT64_MAX;
}

}  // extern "C"

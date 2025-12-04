// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Notifier.h"

#include <sys/types.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "HALInitializer.h"
#include "NotifierInternal.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/Threads.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/handles/UnlimitedHandleResource.h"
#include "wpi/hal/simulation/NotifierData.h"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/Synchronization.h"
#include "wpi/util/priority_queue.hpp"

namespace {
struct Notifier {
  std::string name;
  std::atomic<uint64_t> alarmTime = UINT64_MAX;
  uint64_t intervalTime = 0;
  std::atomic<int32_t> userOverrunCount = 0;
  int32_t overrunCount = 0;
  std::atomic_flag handlerSignaled{};
};
}  // namespace

using namespace wpi::hal;

class NotifierThread : public wpi::util::SafeThread {
 public:
  void Main() override;

  void SetAlarm(HAL_NotifierHandle notifierHandle,
                std::shared_ptr<Notifier>& notifier, uint64_t alarmTime,
                uint64_t intervalTime, bool absolute, int32_t* status);

  void ProcessAlarms(wpi::util::SmallVectorImpl<HAL_NotifierHandle>* signaled);

  bool m_paused = false;

  UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                          HAL_HandleEnum::Notifier>
      m_handles;

  struct Alarm {
    HAL_NotifierHandle handle;
    std::shared_ptr<Notifier> notifier;
    bool operator==(const Alarm& rhs) const { return handle == rhs.handle; }
    bool operator>(const Alarm& rhs) const {
      return notifier->alarmTime > rhs.notifier->alarmTime;
    }
  };
  wpi::util::priority_queue<Alarm, std::vector<Alarm>, std::greater<Alarm>>
      m_alarmQueue;
};

class NotifierInstance {
 public:
  NotifierInstance() { owner.Start(); }
  wpi::util::SafeThreadOwner<NotifierThread> owner;
};

static NotifierInstance* notifierInstance;

namespace wpi::hal::init {
void InitializeNotifier() {
  static NotifierInstance n;
  notifierInstance = &n;
}
}  // namespace wpi::hal::init

void NotifierThread::Main() {
  std::unique_lock lock(m_mutex);
  while (m_active) {
    if (m_paused || m_alarmQueue.empty()) {
      // No alarms, wait indefinitely
      m_cond.wait(lock);
      continue;
    }

    // Wait until next alarm
    const Alarm& alarm = m_alarmQueue.top();
    int32_t status = 0;
    uint64_t curTime = HAL_GetFPGATime(&status);
    if (alarm.notifier->alarmTime > curTime) {
      m_cond.wait_for(
          lock, std::chrono::microseconds{alarm.notifier->alarmTime - curTime});
    }
    if (!m_active) {
      break;
    }

    // Check paused again as we may have been paused while waiting
    if (m_paused) {
      continue;
    }

    ProcessAlarms(nullptr);
  }
}

void NotifierThread::SetAlarm(HAL_NotifierHandle notifierHandle,
                              std::shared_ptr<Notifier>& notifier,
                              uint64_t alarmTime, uint64_t intervalTime,
                              bool absolute, int32_t* status) {
  if (!absolute) {
    alarmTime += HAL_GetFPGATime(status);
  }

  uint64_t prevWakeup = UINT64_MAX;
  if (!m_alarmQueue.empty()) {
    prevWakeup = m_alarmQueue.top().notifier->alarmTime;
    m_alarmQueue.remove({notifierHandle, notifier});
  }
  notifier->alarmTime = alarmTime;
  notifier->intervalTime = intervalTime;
  notifier->overrunCount = 0;
  m_alarmQueue.push({notifierHandle, notifier});

  // wake up notifier thread if needed
  if (alarmTime < prevWakeup) {
    m_cond.notify_all();
  }
}

void NotifierThread::ProcessAlarms(
    wpi::util::SmallVectorImpl<HAL_NotifierHandle>* signaled) {
  int32_t status = 0;
  uint64_t curTime = HAL_GetFPGATime(&status);

  // Process alarms
  while (!m_alarmQueue.empty() &&
         m_alarmQueue.top().notifier->alarmTime <= curTime) {
    Alarm alarm = m_alarmQueue.pop();
    HAL_NotifierHandle handle = alarm.handle;
    Notifier& notifier = *alarm.notifier;

    if (notifier.intervalTime > 0) {
      // Schedule next alarm
      notifier.alarmTime += notifier.intervalTime;
      if (curTime >= notifier.alarmTime) {
        // We missed at least one interval
        int32_t missed = static_cast<int32_t>((curTime - notifier.alarmTime) /
                                              notifier.intervalTime) +
                         1;
        notifier.overrunCount += missed;
        notifier.alarmTime +=
            missed * notifier.intervalTime;  // Skip missed intervals
      }
      // Reinsert into queue
      m_alarmQueue.push(std::move(alarm));
    } else {
      // Disable one-shot alarm
      notifier.alarmTime = UINT64_MAX;
    }

    // If the last call was acknowledged, signal the handler
    if (!notifier.handlerSignaled.test_and_set()) {
      if (signaled) {
        signaled->emplace_back(handle);
      }
      // copy the overrun count for the handler to read, reset the local count
      notifier.userOverrunCount = notifier.overrunCount;
      notifier.overrunCount = 0;
      wpi::util::SetSignalObject(handle);
    }
  }
}

void wpi::hal::PauseNotifiers() {
  auto thr = notifierInstance->owner.GetThread();
  thr->m_paused = true;
}

void wpi::hal::ResumeNotifiers() {
  auto thr = notifierInstance->owner.GetThread();
  thr->m_paused = false;
  thr->m_cond.notify_all();
}

void wpi::hal::WakeupNotifiers() {
  auto thr = notifierInstance->owner.GetThread();
  thr->ProcessAlarms(nullptr);
}

static void DoWaitNotifiers(
    wpi::util::detail::SafeThreadProxy<NotifierThread>& thr,
    wpi::util::SmallVectorImpl<HAL_NotifierHandle>& signaled) {
  // Wait for signaled notifiers to acknowledge their last alarm
  for (;;) {
    signaled.erase(std::remove_if(signaled.begin(), signaled.end(),
                                  [&](HAL_NotifierHandle handle) {
                                    auto notifier = thr->m_handles.Get(handle);
                                    return !notifier ||
                                           !notifier->handlerSignaled.test();
                                  }),
                   signaled.end());
    if (signaled.empty()) {
      break;
    }
    thr->m_cond.wait_for(thr.GetLock(), std::chrono::milliseconds{1});
  }
}

void wpi::hal::WaitNotifiers() {
  auto thr = notifierInstance->owner.GetThread();

  wpi::util::SmallVector<HAL_NotifierHandle, 8> signaled;
  thr->m_handles.ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    if (notifier->handlerSignaled.test()) {
      signaled.emplace_back(handle);
    }
  });
  DoWaitNotifiers(thr, signaled);
}

void wpi::hal::WakeupWaitNotifiers() {
  auto thr = notifierInstance->owner.GetThread();

  wpi::util::SmallVector<HAL_NotifierHandle, 8> signaled;
  thr->ProcessAlarms(&signaled);
  DoWaitNotifiers(thr, signaled);
}

extern "C" {

HAL_NotifierHandle HAL_CreateNotifier(int32_t* status) {
  wpi::hal::init::CheckInit();
  std::shared_ptr<Notifier> notifier = std::make_shared<Notifier>();
  HAL_NotifierHandle handle =
      notifierInstance->owner.GetThread()->m_handles.Allocate(notifier);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  wpi::util::CreateSignalObject(handle);
  return handle;
}

HAL_Bool HAL_SetNotifierThreadPriority(HAL_Bool realTime, int32_t priority,
                                       int32_t* status) {
  auto native = notifierInstance->owner.GetNativeThreadHandle();
  return HAL_SetThreadPriority(&native, realTime, priority, status);
}

void HAL_SetNotifierName(HAL_NotifierHandle notifierHandle,
                         const WPI_String* name, int32_t* status) {
  auto thr = notifierInstance->owner.GetThread();
  auto notifier = thr->m_handles.Get(notifierHandle);
  if (!notifier) {
    return;
  }
  notifier->name = wpi::util::to_string_view(name);
}

void HAL_DestroyNotifier(HAL_NotifierHandle notifierHandle) {
  wpi::util::DestroySignalObject(notifierHandle);
  auto thr = notifierInstance->owner.GetThread();
  auto notifier = thr->m_handles.Free(notifierHandle);
  thr->m_alarmQueue.remove({notifierHandle, notifier});
}

void HAL_SetNotifierAlarm(HAL_NotifierHandle notifierHandle, uint64_t alarmTime,
                          uint64_t intervalTime, HAL_Bool absolute,
                          int32_t* status) {
  auto thr = notifierInstance->owner.GetThread();
  auto notifier = thr->m_handles.Get(notifierHandle);
  if (!notifier) {
    return;
  }
  thr->SetAlarm(notifierHandle, notifier, alarmTime, intervalTime, absolute,
                status);
}

void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status) {
  auto thr = notifierInstance->owner.GetThread();
  auto notifier = thr->m_handles.Get(notifierHandle);
  if (!notifier) {
    return;
  }

  thr->m_alarmQueue.remove({notifierHandle, notifier});
  notifier->alarmTime = UINT64_MAX;
  notifier->handlerSignaled.clear();
}

void HAL_AcknowledgeNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  HAL_Bool setAlarm, uint64_t alarmTime,
                                  uint64_t intervalTime, HAL_Bool absolute,
                                  int32_t* status) {
  auto thr = notifierInstance->owner.GetThread();
  auto notifier = thr->m_handles.Get(notifierHandle);
  if (!notifier) {
    return;
  }
  notifier->handlerSignaled.clear();
  if (setAlarm) {
    thr->SetAlarm(notifierHandle, notifier, alarmTime, intervalTime, absolute,
                  status);
  }
}

int32_t HAL_GetNotifierOverrun(HAL_NotifierHandle notifierHandle,
                               int32_t* status) {
  auto notifier =
      notifierInstance->owner.GetThread()->m_handles.Get(notifierHandle);
  if (!notifier) {
    return -1;
  }
  return notifier->userOverrunCount;
}

uint64_t HALSIM_GetNextNotifierTimeout(void) {
  auto thr = notifierInstance->owner.GetThread();
  if (thr->m_alarmQueue.empty()) {
    return UINT64_MAX;
  }
  return thr->m_alarmQueue.top().notifier->alarmTime;
}

int32_t HALSIM_GetNumNotifiers(void) {
  auto thr = notifierInstance->owner.GetThread();
  int32_t count = 0;
  thr->m_handles.ForEach([&](auto, auto) { ++count; });
  return count;
}

int32_t HALSIM_GetNotifierInfo(struct HALSIM_NotifierInfo* arr, int32_t size) {
  auto thr = notifierInstance->owner.GetThread();
  int32_t num = 0;
  thr->m_handles.ForEach([&](HAL_NotifierHandle handle, Notifier* notifier) {
    if (num < size) {
      arr[num].handle = handle;
      if (notifier->name.empty()) {
        wpi::util::format_to_n_c_str(arr[num].name, sizeof(arr[num].name),
                                     "Notifier{}",
                                     static_cast<int>(getHandleIndex(handle)));
      } else {
        std::strncpy(arr[num].name, notifier->name.c_str(),
                     sizeof(arr[num].name) - 1);
        arr[num].name[sizeof(arr[num].name) - 1] = '\0';
      }
      arr[num].alarmTime = notifier->alarmTime;
      arr[num].intervalTime = notifier->intervalTime;
      arr[num].overrunCount = notifier->overrunCount;
    }
    ++num;
  });
  return num;
}

}  // extern "C"

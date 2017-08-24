/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Notifier.h"

#include <chrono>
#include <iostream>

#include "HAL/HAL.h"
#include "HAL/cpp/fpga_clock.h"
#include "HAL/cpp/make_unique.h"
#include "HAL/handles/UnlimitedHandleResource.h"
#include "support/SafeThread.h"
#include "support/timestamp.h"

namespace {
class NotifierThread : public wpi::SafeThread {
 public:
  void Main() {
    int32_t status = 0;
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_active) {
    startNotifierLoop:
      double waitTime = m_waitTime * 1e-6;
      if (!m_running) {
        status = 0;
        waitTime = (HAL_GetFPGATime(&status) * 1e-6) + 1000.0;
        // If not running, wait 1000 seconds
      }

      auto timeoutTime =
          hal::fpga_clock::epoch() + std::chrono::duration<double>(waitTime);
      // auto timeoutTime = std::chrono::steady_clock::now() +
      // std::chrono::duration<double>(1.0);
      m_cond.wait_until(lock, timeoutTime);
      if (m_updatedAlarm) {
        m_updatedAlarm = false;
        goto startNotifierLoop;
      }
      if (!m_running) continue;
      if (!m_active) break;
      m_running = false;
      int32_t status = 0;
      uint64_t currentTime = HAL_GetFPGATime(&status);
      HAL_NotifierHandle handle = m_handle;
      HAL_NotifierProcessFunction process = m_process;
      lock.unlock();  // don't hold mutex during callback execution
      process(currentTime, handle);
      m_updatedAlarm = false;
      lock.lock();
    }
  }

  HAL_NotifierHandle m_handle = HAL_kInvalidHandle;
  HAL_NotifierProcessFunction m_process;
  uint64_t m_waitTime;
  bool m_updatedAlarm = false;
  bool m_running = false;
};

class NotifierThreadOwner : public wpi::SafeThreadOwner<NotifierThread> {
 public:
  void SetFunc(HAL_NotifierProcessFunction process, HAL_NotifierHandle handle) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_process = process;
    thr->m_handle = handle;
  }

  void UpdateAlarm(uint64_t triggerTime) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_waitTime = triggerTime;
    thr->m_running = true;
    thr->m_updatedAlarm = true;
    thr->m_cond.notify_one();
  }

  void StopAlarm() {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_running = false;
  }
};
struct Notifier {
  std::unique_ptr<NotifierThreadOwner> thread;
  void* param;
};
}  // namespace

using namespace hal;

static UnlimitedHandleResource<HAL_NotifierHandle, Notifier,
                               HAL_HandleEnum::Notifier>
    notifierHandles;

extern "C" {

HAL_NotifierHandle HAL_InitializeNotifierNonThreadedUnsafe(
    HAL_NotifierProcessFunction process, void* param, int32_t* status) {
  return HAL_InitializeNotifier(process, param, status);
}

HAL_NotifierHandle HAL_InitializeNotifier(HAL_NotifierProcessFunction process,
                                          void* param, int32_t* status) {
  if (!process) {
    *status = NULL_PARAMETER;
    return 0;
  }

  std::shared_ptr<Notifier> notifier = std::make_shared<Notifier>();
  HAL_NotifierHandle handle = notifierHandles.Allocate(notifier);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  notifier->thread = std::make_unique<NotifierThreadOwner>();

  notifier->thread->Start();
  notifier->thread->SetFunc(process, handle);

  notifier->param = param;

  return handle;
}

void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return;
  notifier->thread->StopAlarm();
  notifierHandles.Free(notifierHandle);
}
void* HAL_GetNotifierParam(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return nullptr;
  return notifier->param;
}
void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status) {
  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return;
  notifier->thread->UpdateAlarm(triggerTime);
}
void HAL_StopNotifierAlarm(HAL_NotifierHandle notifierHandle, int32_t* status) {
  auto notifier = notifierHandles.Get(notifierHandle);
  if (!notifier) return;
  notifier->thread->StopAlarm();
}

}  // extern "C"

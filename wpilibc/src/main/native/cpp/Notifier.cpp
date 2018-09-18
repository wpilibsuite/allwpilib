/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Notifier.h"

#include <utility>

#include <hal/HAL.h>

#include "frc/Timer.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"

using namespace frc;

Notifier::Notifier(TimerEventHandler handler) {
  if (handler == nullptr)
    wpi_setWPIErrorWithContext(NullParameter, "handler must not be nullptr");
  m_handler = handler;
  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  m_thread = std::thread([=] {
    for (;;) {
      int32_t status = 0;
      HAL_NotifierHandle notifier = m_notifier.load();
      if (notifier == 0) break;
      uint64_t curTime = HAL_WaitForNotifierAlarm(notifier, &status);
      if (curTime == 0 || status != 0) break;

      TimerEventHandler handler;
      {
        std::lock_guard<wpi::mutex> lock(m_processMutex);
        handler = m_handler;
        if (m_periodic) {
          m_expirationTime += m_period;
          UpdateAlarm();
        } else {
          // need to update the alarm to cause it to wait again
          UpdateAlarm(UINT64_MAX);
        }
      }

      // call callback
      if (handler) handler();
    }
  });
}

Notifier::~Notifier() {
  int32_t status = 0;
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_StopNotifier(handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  // Join the thread to ensure the handler has exited.
  if (m_thread.joinable()) m_thread.join();

  HAL_CleanNotifier(handle, &status);
}

Notifier::Notifier(Notifier&& rhs)
    : ErrorBase(std::move(rhs)),
      m_thread(std::move(rhs.m_thread)),
      m_notifier(rhs.m_notifier.load()),
      m_handler(std::move(rhs.m_handler)),
      m_expirationTime(std::move(rhs.m_expirationTime)),
      m_period(std::move(rhs.m_period)),
      m_periodic(std::move(rhs.m_periodic)) {
  rhs.m_notifier = HAL_kInvalidHandle;
}

Notifier& Notifier::operator=(Notifier&& rhs) {
  ErrorBase::operator=(std::move(rhs));

  m_thread = std::move(rhs.m_thread);
  m_notifier = rhs.m_notifier.load();
  rhs.m_notifier = HAL_kInvalidHandle;
  m_handler = std::move(rhs.m_handler);
  m_expirationTime = std::move(rhs.m_expirationTime);
  m_period = std::move(rhs.m_period);
  m_periodic = std::move(rhs.m_periodic);

  return *this;
}

void Notifier::SetHandler(TimerEventHandler handler) {
  std::lock_guard<wpi::mutex> lock(m_processMutex);
  m_handler = handler;
}

void Notifier::StartSingle(double delay) {
  std::lock_guard<wpi::mutex> lock(m_processMutex);
  m_periodic = false;
  m_period = delay;
  m_expirationTime = Timer::GetFPGATimestamp() + m_period;
  UpdateAlarm();
}

void Notifier::StartPeriodic(double period) {
  std::lock_guard<wpi::mutex> lock(m_processMutex);
  m_periodic = true;
  m_period = period;
  m_expirationTime = Timer::GetFPGATimestamp() + m_period;
  UpdateAlarm();
}

void Notifier::Stop() {
  int32_t status = 0;
  HAL_CancelNotifierAlarm(m_notifier, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void Notifier::UpdateAlarm(uint64_t triggerTime) {
  int32_t status = 0;
  // Return if we are being destructed, or were not created successfully
  auto notifier = m_notifier.load();
  if (notifier == 0) return;
  HAL_UpdateNotifierAlarm(notifier, triggerTime, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void Notifier::UpdateAlarm() {
  UpdateAlarm(static_cast<uint64_t>(m_expirationTime * 1e6));
}

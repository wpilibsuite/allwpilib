/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"

#include <HAL/HAL.h>

#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Create a Notifier for timer event notification.
 *
 * @param handler The handler is called at the notification time which is set
 *                using StartSingle or StartPeriodic.
 */
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
        }
      }

      // call callback
      if (handler) handler();
    }
  });
}

/**
 * Free the resources for a timer event.
 */
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

/**
 * Update the HAL alarm time.
 */
void Notifier::UpdateAlarm() {
  int32_t status = 0;
  // Return if we are being destructed, or were not created successfully
  auto notifier = m_notifier.load();
  if (notifier == 0) return;
  HAL_UpdateNotifierAlarm(
      notifier, static_cast<uint64_t>(m_expirationTime * 1e6), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Change the handler function.
 *
 * @param handler Handler
 */
void Notifier::SetHandler(TimerEventHandler handler) {
  std::lock_guard<wpi::mutex> lock(m_processMutex);
  m_handler = handler;
}

/**
 * Register for single event notification.
 *
 * A timer event is queued for a single event after the specified delay.
 *
 * @param delay Seconds to wait before the handler is called.
 */
void Notifier::StartSingle(double delay) {
  std::lock_guard<wpi::mutex> lock(m_processMutex);
  m_periodic = false;
  m_period = delay;
  m_expirationTime = Timer::GetFPGATimestamp() + m_period;
  UpdateAlarm();
}

/**
 * Register for periodic event notification.
 *
 * A timer event is queued for periodic event notification. Each time the
 * interrupt occurs, the event will be immediately requeued for the same time
 * interval.
 *
 * @param period Period in seconds to call the handler starting one period
 *               after the call to this method.
 */
void Notifier::StartPeriodic(double period) {
  std::lock_guard<wpi::mutex> lock(m_processMutex);
  m_periodic = true;
  m_period = period;
  m_expirationTime = Timer::GetFPGATimestamp() + m_period;
  UpdateAlarm();
}

/**
 * Stop timer events from occuring.
 *
 * Stop any repeating timer events from occuring. This will also remove any
 * single notification events from the queue.
 *
 * If a timer-based call to the registered handler is in progress, this function
 * will block until the handler call is complete.
 */
void Notifier::Stop() {
  int32_t status = 0;
  HAL_CancelNotifierAlarm(m_notifier, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"

#include "HAL/HAL.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

using namespace frc;

hal::priority_mutex Notifier::m_destructorMutex;

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
  m_notifier = HAL_InitializeNotifier(&Notifier::Notify, this, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Free the resources for a timer event.
 */
Notifier::~Notifier() {
  int32_t status = 0;
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_CleanNotifier(handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  /* Acquire the mutex; this makes certain that the handler is not being
   * executed by the interrupt manager.
   */
  std::lock_guard<hal::priority_mutex> lockStatic(Notifier::m_destructorMutex);
  std::lock_guard<hal::priority_mutex> lock(m_processMutex);
}

/**
 * Update the HAL alarm time.
 */
void Notifier::UpdateAlarm() {
  int32_t status = 0;
  // Return if we are being destructed, or were not created successfully
  if (m_notifier == 0) return;
  HAL_UpdateNotifierAlarm(
      m_notifier, static_cast<uint64_t>(m_expirationTime * 1e6), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Notify is called by the HAL layer.  We simply need to pass it through to
 * the user handler.
 */
void Notifier::Notify(uint64_t currentTimeInt, HAL_NotifierHandle handle) {
  Notifier* notifier;
  {
    // Lock static mutex to grab the notifier param
    std::lock_guard<hal::priority_mutex> lock(Notifier::m_destructorMutex);
    int32_t status = 0;
    auto notifierPointer = HAL_GetNotifierParam(handle, &status);
    if (notifierPointer == nullptr) return;
    notifier = static_cast<Notifier*>(notifierPointer);
    notifier->m_processMutex.lock();
  }

  if (notifier->m_periodic) {
    notifier->m_expirationTime += notifier->m_period;
    notifier->UpdateAlarm();
  }

  auto handler = notifier->m_handler;

  if (handler) handler();
  notifier->m_processMutex.unlock();
}

/**
 * Register for single event notification.
 *
 * A timer event is queued for a single event after the specified delay.
 *
 * @param delay Seconds to wait before the handler is called.
 */
void Notifier::StartSingle(double delay) {
  std::lock_guard<hal::priority_mutex> sync(m_processMutex);
  m_periodic = false;
  m_period = delay;
  m_expirationTime = GetClock() + m_period;
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
  std::lock_guard<hal::priority_mutex> sync(m_processMutex);
  m_periodic = true;
  m_period = period;
  m_expirationTime = GetClock() + m_period;
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
  HAL_StopNotifierAlarm(m_notifier, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  // Wait for a currently executing handler to complete before returning from
  // Stop()
  std::lock_guard<hal::priority_mutex> lockStatic(Notifier::m_destructorMutex);
  std::lock_guard<hal::priority_mutex> lock(m_processMutex);
}

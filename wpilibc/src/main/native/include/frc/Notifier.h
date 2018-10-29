/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <atomic>
#include <functional>
#include <thread>
#include <utility>

#include <hal/Types.h>
#include <wpi/mutex.h>

#include "frc/ErrorBase.h"

namespace frc {

using TimerEventHandler = std::function<void()>;

class Notifier : public ErrorBase {
 public:
  /**
   * Create a Notifier for timer event notification.
   *
   * @param handler The handler is called at the notification time which is set
   *                using StartSingle or StartPeriodic.
   */
  explicit Notifier(TimerEventHandler handler);

  template <typename Callable, typename Arg, typename... Args>
  Notifier(Callable&& f, Arg&& arg, Args&&... args)
      : Notifier(std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

  /**
   * Free the resources for a timer event.
   */
  virtual ~Notifier();

  Notifier(Notifier&& rhs);
  Notifier& operator=(Notifier&& rhs);

  /**
   * Change the handler function.
   *
   * @param handler Handler
   */
  void SetHandler(TimerEventHandler handler);

  /**
   * Register for single event notification.
   *
   * A timer event is queued for a single event after the specified delay.
   *
   * @param delay Seconds to wait before the handler is called.
   */
  void StartSingle(double delay);

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
  void StartPeriodic(double period);

  /**
   * Stop timer events from occuring.
   *
   * Stop any repeating timer events from occuring. This will also remove any
   * single notification events from the queue.
   *
   * If a timer-based call to the registered handler is in progress, this
   * function will block until the handler call is complete.
   */
  void Stop();

 private:
  /**
   * Update the HAL alarm time.
   *
   * @param triggerTime the time at which the next alarm will be triggered
   */
  void UpdateAlarm(uint64_t triggerTime);

  /**
   * Update the HAL alarm time based on m_expirationTime.
   */
  void UpdateAlarm();

  // The thread waiting on the HAL alarm
  std::thread m_thread;

  // Held while updating process information
  wpi::mutex m_processMutex;

  // HAL handle, atomic for proper destruction
  std::atomic<HAL_NotifierHandle> m_notifier{0};

  // Address of the handler
  TimerEventHandler m_handler;

  // The absolute expiration time
  double m_expirationTime = 0;

  // The relative time (either periodic or single)
  double m_period = 0;

  // True if this is a periodic event
  bool m_periodic = false;
};

}  // namespace frc

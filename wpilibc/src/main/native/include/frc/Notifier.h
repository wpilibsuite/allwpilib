// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <functional>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>

#include <hal/Types.h>
#include <units/time.h>
#include <wpi/mutex.h>

namespace frc {

class Notifier {
 public:
  /**
   * Create a Notifier for timer event notification.
   *
   * @param handler The handler is called at the notification time which is set
   *                using StartSingle or StartPeriodic.
   */
  explicit Notifier(std::function<void()> handler);

  template <
      typename Callable, typename Arg, typename... Args,
      typename = std::enable_if_t<std::is_invocable_v<Callable, Arg, Args...>>>
  Notifier(Callable&& f, Arg&& arg, Args&&... args)
      : Notifier(std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

  /**
   * Create a Notifier for timer event notification.
   *
   * This overload makes the underlying thread run with a real-time priority.
   * This is useful for reducing scheduling jitter on processes which are
   * sensitive to timing variance, like model-based control.
   *
   * @param priority The FIFO real-time scheduler priority ([1..99] where a
   *                 higher number represents higher priority). See "man 7
   *                 sched" for more details.
   * @param handler  The handler is called at the notification time which is set
   *                 using StartSingle or StartPeriodic.
   */
  explicit Notifier(int priority, std::function<void()> handler);

  template <typename Callable, typename Arg, typename... Args>
  Notifier(int priority, Callable&& f, Arg&& arg, Args&&... args)
      : Notifier(priority,
                 std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

  /**
   * Free the resources for a timer event.
   */
  ~Notifier();

  Notifier(Notifier&& rhs);
  Notifier& operator=(Notifier&& rhs);

  /**
   * Sets the name of the notifier.  Used for debugging purposes only.
   *
   * @param name Name
   */
  void SetName(std::string_view name);

  /**
   * Change the handler function.
   *
   * @param handler Handler
   */
  void SetHandler(std::function<void()> handler);

  /**
   * Register for single event notification.
   *
   * A timer event is queued for a single event after the specified delay.
   *
   * @param delay Amount of time to wait before the handler is called.
   */
  void StartSingle(units::second_t delay);

  /**
   * Register for periodic event notification.
   *
   * A timer event is queued for periodic event notification. Each time the
   * interrupt occurs, the event will be immediately requeued for the same time
   * interval.
   *
   * @param period Period to call the handler starting one period
   *               after the call to this method.
   */
  void StartPeriodic(units::second_t period);

  /**
   * Stop timer events from occurring.
   *
   * Stop any repeating timer events from occurring. This will also remove any
   * single notification events from the queue.
   *
   * If a timer-based call to the registered handler is in progress, this
   * function will block until the handler call is complete.
   */
  void Stop();

  /**
   * Sets the HAL notifier thread priority.
   *
   * The HAL notifier thread is responsible for managing the FPGA's notifier
   * interrupt and waking up user's Notifiers when it's their time to run.
   * Giving the HAL notifier thread real-time priority helps ensure the user's
   * real-time Notifiers, if any, are notified to run in a timely manner.
   *
   * @param realTime Set to true to set a real-time priority, false for standard
   *                 priority.
   * @param priority Priority to set the thread to. For real-time, this is 1-99
   *                 with 99 being highest. For non-real-time, this is forced to
   *                 0. See "man 7 sched" for more details.
   * @return         True on success.
   */
  static bool SetHALThreadPriority(bool realTime, int32_t priority);

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
  std::function<void()> m_handler;

  // The absolute expiration time
  units::second_t m_expirationTime = 0_s;

  // The relative time (either periodic or single)
  units::second_t m_period = 0_s;

  // True if this is a periodic event
  bool m_periodic = false;
};

}  // namespace frc

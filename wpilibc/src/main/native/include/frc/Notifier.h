// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <concepts>
#include <functional>
#include <string_view>
#include <thread>
#include <utility>

#include <hal/Types.h>
#include <units/time.h>
#include <wpi/mutex.h>

namespace frc {

/**
 * Notifiers run a user-provided callback function on a separate thread.
 *
 * If StartSingle() is used, the callback will run once. If StartPeriodic() is
 * used, the callback will run repeatedly with the given period until stop() is
 * called.
 */
class Notifier {
 public:
  /**
   * Create a Notifier with the given callback.
   *
   * Configure when the callback runs with StartSingle() or StartPeriodic().
   *
   * @param callback The callback to run.
   */
  explicit Notifier(std::function<void()> callback);

  template <typename Arg, typename... Args>
  Notifier(std::invocable<Arg, Args...> auto&& callback, Arg&& arg,
           Args&&... args)
      : Notifier(std::bind(std::forward<decltype(callback)>(callback),
                           std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

  /**
   * Create a Notifier with the given callback.
   *
   * Configure when the callback runs with StartSingle() or StartPeriodic().
   *
   * This overload makes the underlying thread run with a real-time priority.
   * This is useful for reducing scheduling jitter on processes which are
   * sensitive to timing variance, like model-based control.
   *
   * @param priority The FIFO real-time scheduler priority ([1..99] where a
   *                 higher number represents higher priority). See "man 7
   *                 sched" for more details.
   * @param callback The callback to run.
   */
  explicit Notifier(int priority, std::function<void()> callback);

  template <typename Arg, typename... Args>
  Notifier(int priority, std::invocable<Arg, Args...> auto&& callback,
           Arg&& arg, Args&&... args)
      : Notifier(priority, std::bind(std::forward<decltype(callback)>(callback),
                                     std::forward<Arg>(arg),
                                     std::forward<Args>(args)...)) {}

  /**
   * Free the resources for a timer event.
   */
  ~Notifier();

  Notifier(Notifier&& rhs);
  Notifier& operator=(Notifier&& rhs);

  /**
   * Sets the name of the notifier. Used for debugging purposes only.
   *
   * @param name Name
   */
  void SetName(std::string_view name);

  /**
   * Change the callback function.
   *
   * @param callback The callback function.
   */
  void SetCallback(std::function<void()> callback);

  /**
   * Run the callback once after the given delay.
   *
   * @param delay Time to wait before the callback is called.
   */
  void StartSingle(units::second_t delay);

  /**
   * Run the callback periodically with the given period.
   *
   * The user-provided callback should be written so that it completes before
   * the next time it's scheduled to run.
   *
   * @param period Period after which to to call the callback starting one
   *               period after the call to this method.
   */
  void StartPeriodic(units::second_t period);

  /**
   * Stop further callback invocations.
   *
   * No further periodic callbacks will occur. Single invocations will also be
   * cancelled if they haven't yet occurred.
   *
   * If a callback invocation is in progress, this function will block until the
   * callback is complete.
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

  // The mutex held while updating process information
  wpi::mutex m_processMutex;

  // HAL handle (atomic for proper destruction)
  std::atomic<HAL_NotifierHandle> m_notifier{0};

  // The user-provided callback
  std::function<void()> m_callback;

  // The time at which the callback should be called. Has the same zero as
  // Timer::GetFPGATimestamp().
  units::second_t m_expirationTime = 0_s;

  // If periodic, stores the callback period; if single, stores the time until
  // the callback call.
  units::second_t m_period = 0_s;

  // True if the callback is periodic
  bool m_periodic = false;
};

}  // namespace frc

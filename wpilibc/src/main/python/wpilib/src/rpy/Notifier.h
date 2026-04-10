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

#include "wpi/hal/Types.h"
#include "wpi/units/time.hpp"
#include "wpi/util/mutex.hpp"

#include <semiwrap.h>

namespace wpi {

class PyNotifier {
 public:
  /**
   * Create a Notifier for timer event notification.
   *
   * @param handler The handler is called at the notification time which is set
   *                using StartSingle or StartPeriodic.
   */
  explicit PyNotifier(std::function<void()> handler);

  //   template <typename Callable, typename Arg, typename... Args>
  //   PyNotifier(Callable &&f, Arg &&arg, Args &&... args)
  //       : PyNotifier(std::bind(std::forward<Callable>(f),
  //       std::forward<Arg>(arg),
  //                              std::forward<Args>(args)...)) {}

  /**
   * Free the resources for a timer event.
   */
  virtual ~PyNotifier();

  PyNotifier(PyNotifier &&rhs);
  PyNotifier &operator=(PyNotifier &&rhs);

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
  void SetCallback(std::function<void()> handler);

  /**
   * Register for single event notification.
   *
   * A timer event is queued for a single event after the specified delay.
   *
   * @param delay Amount of time to wait before the handler is called.
   */
  void StartSingle(wpi::units::second_t delay);

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
  void StartPeriodic(wpi::units::second_t period);

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
   * Gets the overrun count.
   *
   * An overrun occurs when a notifier's alarm is not serviced before the next
   * scheduled alarm time.
   *
   * @return overrun count
   */
  int32_t GetOverrun() const;

private:
  // The thread waiting on the HAL alarm
  py::object m_thread;

  // Held while updating process information
  wpi::util::mutex m_processMutex;

  // HAL handle, atomic for proper destruction
  std::atomic<HAL_NotifierHandle> m_notifier{0};

  // Address of the handler
  std::function<void()> m_handler;
};

} // namespace wpi

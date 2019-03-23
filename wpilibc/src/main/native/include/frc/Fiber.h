/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <functional>
#include <utility>

#include <hal/cpp/fpga_clock.h>

namespace frc {

class FiberScheduler;

/**
 * A wrapper around a callable object with an execution period.
 *
 * FiberScheduler can be used to run it periodically.
 */
class Fiber {
 public:
  /**
   * Fiber constructor.
   *
   * @param period   The period between callback invocations in seconds with
   *                 microsecond resolution.
   * @param callback The function to call every period.
   */
  Fiber(double period, std::function<void()> callback);

  template <typename Callable, typename Arg, typename... Args>
  Fiber(double period, Callable&& f, Arg&& arg, Args&&... args)
      : Fiber(period,
              std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                        std::forward<Args>(args)...)) {}

  Fiber(Fiber&&) = default;
  Fiber& operator=(Fiber&&) = default;

 private:
  std::chrono::microseconds m_period;
  hal::fpga_clock::time_point m_expirationTime;
  std::function<void()> m_callback;

  bool operator>(const Fiber& rhs);

  friend class FiberScheduler;
};

}  // namespace frc

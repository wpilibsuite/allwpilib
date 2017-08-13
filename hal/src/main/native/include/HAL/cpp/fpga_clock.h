/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <limits>

namespace hal {

class fpga_clock {
 public:
  typedef std::chrono::microseconds::rep rep;
  typedef std::chrono::microseconds::period period;
  typedef std::chrono::microseconds duration;
  typedef std::chrono::time_point<fpga_clock> time_point;

  static fpga_clock::time_point now() noexcept;
  static constexpr bool is_steady = true;

  static constexpr fpga_clock::time_point epoch() { return time_point(zero()); }

  static constexpr fpga_clock::duration zero() { return duration(0); }

  static constexpr time_point min_time{
      time_point(duration(std::numeric_limits<duration::rep>::min()))};
};
}  // namespace hal

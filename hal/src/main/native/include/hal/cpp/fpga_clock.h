/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <limits>

/** WPILib Hardware Abstraction Layer (HAL) namespace */
namespace hal {

/**
 * A std::chrono compatible wrapper around the FPGA Timer.
 */
class fpga_clock {
 public:
  using rep = std::chrono::microseconds::rep;
  using period = std::chrono::microseconds::period;
  using duration = std::chrono::microseconds;
  using time_point = std::chrono::time_point<fpga_clock>;

  static fpga_clock::time_point now() noexcept;
  static constexpr bool is_steady = true;

  static fpga_clock::time_point epoch() noexcept { return time_point(zero()); }

  static fpga_clock::duration zero() noexcept { return duration(0); }

  static const time_point min_time;
};
}  // namespace hal

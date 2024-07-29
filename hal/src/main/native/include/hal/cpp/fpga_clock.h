// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>

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

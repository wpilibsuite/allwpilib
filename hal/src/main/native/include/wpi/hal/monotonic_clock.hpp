// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <limits>

#include "wpi/hal/HAL.h"

/** WPILib Hardware Abstraction Layer (HAL) namespace */
namespace wpi::hal {

/**
 * A std::chrono compatible wrapper around the HAL monotonic timer.
 */
class monotonic_clock {
 public:
  using rep = std::chrono::microseconds::rep;
  using period = std::chrono::microseconds::period;
  using duration = std::chrono::microseconds;
  using time_point = std::chrono::time_point<monotonic_clock>;

  static time_point now() noexcept {
    uint64_t currentTime = HAL_GetMonotonicTime();
    return time_point(std::chrono::microseconds(currentTime));
  }
  static constexpr bool is_steady = true;

  static time_point epoch() noexcept { return time_point(zero()); }

  static duration zero() noexcept { return duration(0); }

  static constexpr time_point min_time =
      time_point(duration(std::numeric_limits<duration::rep>::min()));
};
}  // namespace wpi::hal

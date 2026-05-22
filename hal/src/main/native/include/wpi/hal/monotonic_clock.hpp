// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>

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

  static constexpr bool is_steady = true;

  static time_point now() noexcept {
    uint64_t currentTime = HAL_GetMonotonicTime();
    return time_point{std::chrono::microseconds{currentTime}};
  }
};

}  // namespace wpi::hal

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/cpp/fpga_clock.h"

#include <cstdio>
#include <limits>

#include <wpi/print.h>

#include "hal/HALBase.h"

namespace hal {
const fpga_clock::time_point fpga_clock::min_time =
    fpga_clock::time_point(fpga_clock::duration(
        std::numeric_limits<fpga_clock::duration::rep>::min()));

fpga_clock::time_point fpga_clock::now() noexcept {
  int32_t status = 0;
  uint64_t currentTime = HAL_GetFPGATime(&status);
  if (status != 0) {
    wpi::print(
        stderr,
        "Call to HAL_GetFPGATime failed in fpga_clock::now() with status {}. "
        "Initialization might have failed. Time will not be correct\n",
        status);
    std::fflush(stderr);
    return epoch();
  }
  return time_point(std::chrono::microseconds(currentTime));
}
}  // namespace hal

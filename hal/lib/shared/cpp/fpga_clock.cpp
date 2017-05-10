/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/cpp/fpga_clock.h"

#include "HAL/HAL.h"

namespace hal {
constexpr fpga_clock::time_point fpga_clock::min_time;

fpga_clock::time_point fpga_clock::now() noexcept {
  int32_t status = 0;
  uint64_t currentTime = HAL_GetFPGATime(&status);
  if (status != 0) return epoch();
  return time_point(std::chrono::microseconds(currentTime));
}
}  // namespace hal

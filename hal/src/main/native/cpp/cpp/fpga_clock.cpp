/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/cpp/fpga_clock.h"

#include <limits>

#include <wpi/raw_ostream.h>

#include "hal/HALBase.h"

namespace hal {
const fpga_clock::time_point fpga_clock::min_time =
    fpga_clock::time_point(fpga_clock::duration(
        std::numeric_limits<fpga_clock::duration::rep>::min()));

fpga_clock::time_point fpga_clock::now() noexcept {
  int32_t status = 0;
  uint64_t currentTime = HAL_GetFPGATime(&status);
  if (status != 0) {
    wpi::errs()
        << "Call to HAL_GetFPGATime failed in fpga_clock::now() with status "
        << status
        << ". Initialization might have failed. Time will not be correct\n";
    wpi::errs().flush();
    return epoch();
  }
  return time_point(std::chrono::microseconds(currentTime));
}
}  // namespace hal

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Processes.h"

#include <hal/Processes.h>

#include "frc/Errors.h"

namespace frc {

bool SetProcessPriority(std::string_view process, bool realTime, int priority) {
  int32_t status = 0;
  auto ret =
      HAL_SetProcessPriority(process.data(), realTime, priority, &status);
  FRC_CheckErrorStatus(status, "{}", "SetProcessPriority");
  return ret;
}

}  // namespace frc

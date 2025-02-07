// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/UsageReporting.h"

#include <fmt/format.h>

int32_t HAL_ReportUsage(std::string_view resource, int instanceNumber,
                        std::string_view data) {
  return HAL_ReportUsage(fmt::format("{}[{}]", resource, instanceNumber), data);
}

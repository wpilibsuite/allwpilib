// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/UsageReporting.h"

#include <string>

#include <fmt/format.h>

int32_t HAL_ReportUsage(std::string_view resource, int instanceNumber,
                        std::string_view data) {
  return HAL_ReportUsage(fmt::format("{}[{}]", resource, instanceNumber), data);
}

int32_t HAL_ReportUsageCount(std::string_view resource, int count) {
  return HAL_ReportUsage(resource, std::to_string(count));
}

int32_t HAL_ReportUsageCount(std::string_view resource, int instanceNumber,
                             int count) {
  return HAL_ReportUsage(resource, instanceNumber, std::to_string(count));
}

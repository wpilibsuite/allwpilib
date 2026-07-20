// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/hal/UsageReporting.h"
#include "wpi/util/string.hpp"

/**
 * Reports usage of a resource of interest.  Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource       the used resource name; convention is to suffix with
 *                       "[instanceNum]" for multiple instances of the same
 *                       resource
 * @param data           arbitrary associated data string
 */
inline void HAL_ReportUsage(std::string_view resource, std::string_view data) {
  WPI_String resourceStr = wpi::util::make_string(resource);
  WPI_String dataStr = wpi::util::make_string(data);
  HAL_ReportUsage(&resourceStr, &dataStr);
}

/** Publishes a version for a device on a CAN bus. */
inline int32_t HAL_PublishCanVersion(uint8_t busId, uint32_t deviceId,
                                     std::string_view name,
                                     std::string_view version) {
  WPI_String nameStr = wpi::util::make_string(name);
  WPI_String versionStr = wpi::util::make_string(version);
  return HAL_PublishCanVersion(busId, deviceId, &nameStr, &versionStr);
}

/** Publishes a version without a bus or device identifier. */
inline int32_t HAL_PublishVersion(std::string_view name,
                                  std::string_view version) {
  WPI_String nameStr = wpi::util::make_string(name);
  WPI_String versionStr = wpi::util::make_string(version);
  return HAL_PublishVersion(&nameStr, &versionStr);
}

/** Publishes the WPILib version. */
inline int32_t HAL_PublishWpilibVersion(std::string_view version) {
  WPI_String versionStr = wpi::util::make_string(version);
  return HAL_PublishWpilibVersion(&versionStr);
}

/**
 * Reports usage of a resource of interest.  Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource       the used resource name
 * @param instanceNumber an index that identifies the resource instance
 * @param data           arbitrary associated data string
 */
void HAL_ReportUsage(std::string_view resource, int instanceNumber,
                     std::string_view data);

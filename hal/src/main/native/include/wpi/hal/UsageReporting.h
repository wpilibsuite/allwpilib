// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/util/string.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reports usage of a resource of interest.  Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource       the used resource name; convention is to suffix with
 *                       "[instanceNum]" for multiple instances of the same
 *                       resource
 * @param data           arbitrary associated data string
 */
void HAL_ReportUsage(const struct WPI_String* resource,
                     const struct WPI_String* data);

/**
 * Publishes a version for a device on a CAN bus.
 *
 * @param busId bus containing the device
 * @param deviceId device identifier
 * @param name version name
 * @param version version string
 * @return status of the operation
 */
int32_t HAL_PublishCanVersion(uint8_t busId, uint32_t deviceId,
                              const struct WPI_String* name,
                              const struct WPI_String* version);

/**
 * Publishes a version without a bus or device identifier.
 *
 * The version is published on bus 31 for device 0.
 *
 * @param name version name
 * @param version version string
 * @return status of the operation
 */
int32_t HAL_PublishVersion(const struct WPI_String* name,
                           const struct WPI_String* version);

/**
 * Publishes the WPILib version.
 *
 * @param version WPILib version string
 * @return status of the operation
 */
int32_t HAL_PublishWpilibVersion(const struct WPI_String* version);

#ifdef __cplusplus
}  // extern "C"
#endif

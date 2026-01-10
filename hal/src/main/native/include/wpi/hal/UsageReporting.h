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
 * @return               a handle
 */
int32_t HAL_ReportUsage(const struct WPI_String* resource,
                        const struct WPI_String* data);

#ifdef __cplusplus
}  // extern "C"
#endif

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/util/string.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Function pointer for usage reporting implementations. */
typedef void (*WPI_ReportUsageImpl)(const struct WPI_String* resource,
                                    const struct WPI_String* data);

/**
 * Reports usage of a resource of interest. Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource the used resource name; convention is to suffix with
 *                 "[instanceNum]" for multiple instances of the same resource
 * @param data arbitrary associated data string
 */
void WPI_ReportUsage(const struct WPI_String* resource,
                     const struct WPI_String* data);

/**
 * Reports usage of a resource of interest. Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource the used resource name
 * @param instanceNumber an index that identifies the resource instance
 * @param data arbitrary associated data string
 */
void WPI_ReportUsageInstance(const struct WPI_String* resource,
                             int32_t instanceNumber,
                             const struct WPI_String* data);

/**
 * Sets the implementation used by WPI_ReportUsage().
 *
 * @param func function called by WPI_ReportUsage(); pass NULL to restore the
 *             default no-op implementation
 */
void WPI_SetReportUsageImpl(WPI_ReportUsageImpl func);

#ifdef __cplusplus
}  // extern "C"
#endif

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <string_view>
#endif

#include <wpi/string.h>

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

#ifdef __cplusplus
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
inline int32_t HAL_ReportUsage(std::string_view resource,
                               std::string_view data) {
  WPI_String resourceStr = wpi::make_string(resource);
  WPI_String dataStr = wpi::make_string(data);
  return HAL_ReportUsage(&resourceStr, &dataStr);
}

/**
 * Reports usage of a resource of interest.  Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource       the used resource name
 * @param instanceNumber an index that identifies the resource instance
 * @param data           arbitrary associated data string
 * @return               a handle
 */
int32_t HAL_ReportUsage(std::string_view resource, int instanceNumber,
                        std::string_view data);

/**
 * Reports usage of a resource of interest.  Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource       the used resource name; convention is to suffix with
 *                       "[instanceNum]" for multiple instances of the same
 *                       resource
 * @param count          number of times this resource has been used
 * @return               a handle
 */
int32_t HAL_ReportUsageCount(std::string_view resource, int count);

/**
 * Reports usage of a resource of interest.  Repeated calls for the same
 * resource name replace the previous report.
 *
 * @param resource       the used resource name
 * @param instanceNumber an index that identifies the resource instance
 * @param count          number of times this resource has been used
 * @return               a handle
 */
int32_t HAL_ReportUsageCount(std::string_view resource, int instanceNumber,
                             int count);
#endif

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/UsageReporting.h"

#include <stdint.h>

#include "mrclib/Reporting.h"

namespace {
MRC_String ToMrcString(const WPI_String* wpiString) {
  if (wpiString == nullptr) {
    return MRC_String{nullptr, 0};
  }
  return MRC_String{wpiString->str, wpiString->len};
}
}  // namespace

extern "C" {

void HAL_ReportUsage(const struct WPI_String* resource,
                     const struct WPI_String* data) {
  MRC_String mrcResource = ToMrcString(resource);
  MRC_String mrcData = ToMrcString(data);
  MRC_Reporting_ReportUsage(&mrcResource, &mrcData);
}

int32_t HAL_PublishCanVersion(uint8_t busId, uint32_t deviceId,
                              const struct WPI_String* name,
                              const struct WPI_String* version) {
  MRC_String mrcName = ToMrcString(name);
  MRC_String mrcVersion = ToMrcString(version);
  return MRC_Reporting_PublishCanVersion(busId, deviceId, &mrcName,
                                         &mrcVersion);
}

int32_t HAL_PublishVersion(const struct WPI_String* name,
                           const struct WPI_String* version) {
  MRC_String mrcName = ToMrcString(name);
  MRC_String mrcVersion = ToMrcString(version);
  return MRC_Reporting_PublishVersion(&mrcName, &mrcVersion);
}

int32_t HAL_PublishWpilibVersion(const struct WPI_String* version) {
  MRC_String mrcVersion = ToMrcString(version);
  return MRC_Reporting_PublishWpilibVersion(&mrcVersion);
}

}  // extern "C"

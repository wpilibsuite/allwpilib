// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Power.h"

#include <cmath>

#include "mockdata/RoboRioDataInternal.hpp"
#include "mrclib/Systemcore.h"
#include "wpi/hal/Errors.h"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializePower() {}
}  // namespace wpi::hal::init

// TODO: Fix the naming in here
extern "C" {
double HAL_GetVinVoltage(int32_t* status) {
  return SimRoboRioData->vInVoltage;
}
double HAL_GetUserVoltage3V3(int32_t* status) {
  return SimRoboRioData->userVoltage3V3;
}
double HAL_GetUserCurrent3V3(int32_t* status) {
  return SimRoboRioData->userCurrent3V3;
}
HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  return SimRoboRioData->userActive3V3;
}
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  return SimRoboRioData->userFaults3V3;
}
void HAL_SetUserRailEnabled3V3(HAL_Bool enabled, int32_t* status) {}
void HAL_ResetUserCurrentFaults(int32_t* status) {
  SimRoboRioData->userFaults3V3 = 0;
}
void HAL_SetBrownoutVoltages(double brownoutVoltage, double recoveryVoltage,
                             int32_t* status) {
  constexpr double kMillivoltsPerVolt = 1000.0;
  constexpr double kBrownoutVoltageMin =
      MRC_SYSTEMCORE_BROWNOUT_VOLTAGE_MIN_MV / kMillivoltsPerVolt;
  constexpr double kBrownoutVoltageMax =
      MRC_SYSTEMCORE_BROWNOUT_VOLTAGE_MAX_MV / kMillivoltsPerVolt;
  constexpr double kRecoveryVoltageMax =
      MRC_SYSTEMCORE_BROWNOUT_RECOVERY_VOLTAGE_MAX_MV /
      kMillivoltsPerVolt;
  constexpr double kRecoveryVoltageMinDelta =
      MRC_SYSTEMCORE_BROWNOUT_RECOVERY_VOLTAGE_MIN_DELTA_MV /
      kMillivoltsPerVolt;
  if (!std::isfinite(brownoutVoltage) || !std::isfinite(recoveryVoltage) ||
      brownoutVoltage < kBrownoutVoltageMin ||
      brownoutVoltage > kBrownoutVoltageMax ||
      recoveryVoltage > kRecoveryVoltageMax ||
      recoveryVoltage < brownoutVoltage + kRecoveryVoltageMinDelta) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  SimRoboRioData->brownoutVoltage = brownoutVoltage;
  SimRoboRioData->brownoutRecoveryVoltage = recoveryVoltage;
  *status = HAL_SUCCESS;
}
double HAL_GetCPUTemp(int32_t* status) {
  return SimRoboRioData->cpuTemp;
}
}  // extern "C"

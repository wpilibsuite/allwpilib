// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Power.h"

#include <cmath>
#include <mutex>

#include "mockdata/RoboRioDataInternal.hpp"
#if __has_include("mrclib/Systemcore.h")
#include "mrclib/Systemcore.h"
#define WPI_HAL_SIM_HAS_MRCLIB_SYSTEMCORE_H
#endif
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
  constexpr double MILLIVOLTS_PER_VOLT = 1000.0;
  constexpr int32_t BROWNOUT_VOLTAGE_MIN_MILLIVOLTS = 5000;
  constexpr int32_t BROWNOUT_VOLTAGE_MAX_MILLIVOLTS = 8000;
  constexpr int32_t RECOVERY_VOLTAGE_MAX_MILLIVOLTS = 8500;
  constexpr int32_t RECOVERY_VOLTAGE_MIN_DELTA_MILLIVOLTS = 500;
#ifdef WPI_HAL_SIM_HAS_MRCLIB_SYSTEMCORE_H
  static_assert(BROWNOUT_VOLTAGE_MIN_MILLIVOLTS ==
                MRC_SYSTEMCORE_BROWNOUT_VOLTAGE_MIN_MV);
  static_assert(BROWNOUT_VOLTAGE_MAX_MILLIVOLTS ==
                MRC_SYSTEMCORE_BROWNOUT_VOLTAGE_MAX_MV);
  static_assert(RECOVERY_VOLTAGE_MAX_MILLIVOLTS ==
                MRC_SYSTEMCORE_BROWNOUT_RECOVERY_VOLTAGE_MAX_MV);
  static_assert(RECOVERY_VOLTAGE_MIN_DELTA_MILLIVOLTS ==
                MRC_SYSTEMCORE_BROWNOUT_RECOVERY_VOLTAGE_MIN_DELTA_MV);
#endif
  constexpr double BROWNOUT_VOLTAGE_MIN =
      BROWNOUT_VOLTAGE_MIN_MILLIVOLTS / MILLIVOLTS_PER_VOLT;
  constexpr double BROWNOUT_VOLTAGE_MAX =
      BROWNOUT_VOLTAGE_MAX_MILLIVOLTS / MILLIVOLTS_PER_VOLT;
  constexpr double RECOVERY_VOLTAGE_MAX =
      RECOVERY_VOLTAGE_MAX_MILLIVOLTS / MILLIVOLTS_PER_VOLT;
  if (!std::isfinite(brownoutVoltage) || !std::isfinite(recoveryVoltage) ||
      brownoutVoltage < BROWNOUT_VOLTAGE_MIN ||
      brownoutVoltage > BROWNOUT_VOLTAGE_MAX ||
      recoveryVoltage < BROWNOUT_VOLTAGE_MIN ||
      recoveryVoltage > RECOVERY_VOLTAGE_MAX) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  auto brownoutMillivolts = std::lround(brownoutVoltage * MILLIVOLTS_PER_VOLT);
  auto recoveryMillivolts = std::lround(recoveryVoltage * MILLIVOLTS_PER_VOLT);
  if (recoveryMillivolts <
      brownoutMillivolts + RECOVERY_VOLTAGE_MIN_DELTA_MILLIVOLTS) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  auto& brownoutValue = SimRoboRioData->brownoutVoltage;
  auto& recoveryValue = SimRoboRioData->brownoutRecoveryVoltage;
  std::scoped_lock lock{brownoutValue.GetMutex(), recoveryValue.GetMutex()};
  bool brownoutChanged =
      brownoutValue.SetNoNotify(brownoutMillivolts / MILLIVOLTS_PER_VOLT);
  bool recoveryChanged =
      recoveryValue.SetNoNotify(recoveryMillivolts / MILLIVOLTS_PER_VOLT);
  if (brownoutChanged) {
    brownoutValue.Notify();
  }
  if (recoveryChanged) {
    recoveryValue.Notify();
  }
  *status = HAL_SUCCESS;
}
double HAL_GetCPUTemp(int32_t* status) {
  return SimRoboRioData->cpuTemp;
}
}  // extern "C"

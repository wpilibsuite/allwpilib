// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Power.h"

#include <cmath>

#include "HALInitializer.hpp"
#include "SystemServerInternal.hpp"
#include "mrclib/Systemcore.h"
#include "wpi/hal/Errors.h"
#include "wpi/nt/DoubleTopic.hpp"

using namespace wpi::hal;

namespace wpi::hal {

static void initializePower(int32_t* status) {
  wpi::hal::init::CheckInit();
}

}  // namespace wpi::hal

namespace wpi::hal::init {
void InitializePower() {}
}  // namespace wpi::hal::init

extern "C" {

double HAL_GetVinVoltage(int32_t* status) {
  float voltage = 0;
  MRC_Status mrcStatus = MRC_Systemcore_GetBatteryVoltage(&voltage);
  if (mrcStatus != MRC_STATUS_SUCCESS) {
    *status = HAL_INCOMPATIBLE_STATE;
    return 0;
  }
  *status = HAL_SUCCESS;
  return voltage;
}

double HAL_GetUserVoltage3V3(int32_t* status) {
  initializePower(status);
  // Until we have a value, make this work, as lots of other
  // code depends on it.
  return 3.3;
}

double HAL_GetUserCurrent3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetUserRailEnabled3V3(HAL_Bool enabled, int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ResetUserCurrentFaults(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetBrownoutVoltages(double brownoutVoltage, double recoveryVoltage,
                             int32_t* status) {
  initializePower(status);
  constexpr double MILLIVOLTS_PER_VOLT = 1000.0;
  constexpr double BROWNOUT_VOLTAGE_MIN =
      MRC_SYSTEMCORE_BROWNOUT_VOLTAGE_MIN_MV / MILLIVOLTS_PER_VOLT;
  constexpr double BROWNOUT_VOLTAGE_MAX =
      MRC_SYSTEMCORE_BROWNOUT_VOLTAGE_MAX_MV / MILLIVOLTS_PER_VOLT;
  constexpr double RECOVERY_VOLTAGE_MAX =
      MRC_SYSTEMCORE_BROWNOUT_RECOVERY_VOLTAGE_MAX_MV / MILLIVOLTS_PER_VOLT;
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
      brownoutMillivolts +
          MRC_SYSTEMCORE_BROWNOUT_RECOVERY_VOLTAGE_MIN_DELTA_MV) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  MRC_Status mrcStatus = MRC_Systemcore_SetBrownoutVoltages(brownoutMillivolts,
                                                            recoveryMillivolts);
  if (mrcStatus == MRC_STATUS_PARAMETER_OUT_OF_RANGE) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
  } else if (mrcStatus != MRC_STATUS_SUCCESS) {
    *status = HAL_INCOMPATIBLE_STATE;
  } else {
    *status = HAL_SUCCESS;
  }
}

double HAL_GetCPUTemp(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"

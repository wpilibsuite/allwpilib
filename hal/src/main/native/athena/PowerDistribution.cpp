// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PowerDistribution.h"

#include "CTREPDP.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t moduleNumber, HAL_PowerDistributionType type, int32_t* status) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kAutomatic) {
    type = HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE;
  }

  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    return static_cast<HAL_PowerDistributionHandle>(
        HAL_InitializePDP(moduleNumber, nullptr, status));  // TODO
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return HAL_kInvalidHandle;
  }
}

#define IsCtre(handle) ::hal::isHandleType(handle, HAL_HandleEnum::CTREPDP)

void HAL_CleanPowerDistribution(HAL_PowerDistributionHandle handle) {
  if (IsCtre(handle)) {
    HAL_CleanPDP(handle);
  } else {
    // TODO
  }
}

HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel) {
  if (IsCtre(handle)) {
    return HAL_CheckPDPChannel(channel);
  } else {
    return false;
    // TODO
  }
}

HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    return HAL_CheckPDPModule(module);
  } else {
    return false;
    // TODO
  }
}

HAL_PowerDistributionType HAL_GetPowerDistributionType(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  return IsCtre(handle)
             ? HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE
             : HAL_PowerDistributionType::HAL_PowerDistributionType_kRev;
}

double HAL_GetPowerDistributionTemperature(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTemperature(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return false;
  }
}

double HAL_GetPowerDistributionVoltage(HAL_PowerDistributionHandle handle,
                                       int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPVoltage(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return false;
  }
}

double HAL_GetPowerDistributionChannelCurrent(
    HAL_PowerDistributionHandle handle, int32_t channel, int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPChannelCurrent(handle, channel, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return 0;
  }
}

void HAL_GetPowerDistributionAllChannelCurrents(
    HAL_PowerDistributionHandle handle, double* currents,
    int32_t currentsLength, int32_t* status) {
  if (IsCtre(handle)) {
    if (currentsLength < kNumPDPChannels) {
      *status = PARAMETER_OUT_OF_RANGE;
      SetLastError(status, "Output array not large enough");
      return;
    }
    return HAL_GetPDPAllChannelCurrents(handle, currents, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
  }
}

double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalCurrent(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return 0;
  }
}

double HAL_GetPowerDistributionTotalPower(HAL_PowerDistributionHandle handle,
                                          int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalPower(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return 0;
  }
}

double HAL_GetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalEnergy(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
    return 0;
  }
}

void HAL_ResetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  if (IsCtre(handle)) {
    HAL_ResetPDPTotalEnergy(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
  }
}

void HAL_ClearPowerDistributionStickyFaults(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  if (IsCtre(handle)) {
    HAL_ClearPDPStickyFaults(handle, status);
  } else {
    *status = PARAMETER_OUT_OF_RANGE;
    SetLastError(status, "Rev Power not currently supported");
  }
}
}  // extern "C"

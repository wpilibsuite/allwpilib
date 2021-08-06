// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PowerDistribution.h"

#include "CTREPDP.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "REVPDH.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t moduleNumber, HAL_PowerDistributionType type, int32_t* status) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kAutomatic) {
    type = HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE;
    moduleNumber = 0;
  }

  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    return static_cast<HAL_PowerDistributionHandle>(
        HAL_InitializePDP(moduleNumber, nullptr, status));  // TODO
  } else {
    return static_cast<HAL_PowerDistributionHandle>(
        HAL_REV_InitializePDH(moduleNumber, nullptr, status));
  }
}

#define IsCtre(handle) ::hal::isHandleType(handle, HAL_HandleEnum::CTREPDP)

void HAL_CleanPowerDistribution(HAL_PowerDistributionHandle handle) {
  if (IsCtre(handle)) {
    HAL_CleanPDP(handle);
  } else {
    HAL_REV_FreePDH(handle);
  }
}

int32_t HAL_GetPowerDistributionModuleNumber(HAL_PowerDistributionHandle handle,
                                             int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPModuleNumber(handle, status);
  } else {
    return HAL_REV_GetPDHModuleNumber(handle, status);
  }
}

HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel) {
  if (IsCtre(handle)) {
    return HAL_CheckPDPChannel(channel);
  } else {
    return HAL_REV_CheckPDHChannelNumber(channel);
  }
}

HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    return HAL_CheckPDPModule(module);
  } else {
    return HAL_REV_CheckPDHModuleNumber(module);
  }
}

HAL_PowerDistributionType HAL_GetPowerDistributionType(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  return IsCtre(handle)
             ? HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE
             : HAL_PowerDistributionType::HAL_PowerDistributionType_kRev;
}

int32_t HAL_GetPowerDistributionNumChannels(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  if (IsCtre(handle)) {
    return kNumCTREPDPChannels;
  } else {
    return kNumREVPDHChannels;
  }
}

double HAL_GetPowerDistributionTemperature(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTemperature(handle, status);
  } else {
    // Not supported
    return 0;
  }
}

double HAL_GetPowerDistributionVoltage(HAL_PowerDistributionHandle handle,
                                       int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPVoltage(handle, status);
  } else {
    return HAL_REV_GetPDHSupplyVoltage(handle, status);
  }
}

double HAL_GetPowerDistributionChannelCurrent(
    HAL_PowerDistributionHandle handle, int32_t channel, int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPChannelCurrent(handle, channel, status);
  } else {
    return HAL_REV_GetPDHChannelCurrent(handle, channel, status);
  }
}

void HAL_GetPowerDistributionAllChannelCurrents(
    HAL_PowerDistributionHandle handle, double* currents,
    int32_t currentsLength, int32_t* status) {
  if (IsCtre(handle)) {
    if (currentsLength < kNumCTREPDPChannels) {
      *status = PARAMETER_OUT_OF_RANGE;
      SetLastError(status, "Output array not large enough");
      return;
    }
    return HAL_GetPDPAllChannelCurrents(handle, currents, status);
  } else {
    if (currentsLength < kNumREVPDHChannels) {
      *status = PARAMETER_OUT_OF_RANGE;
      SetLastError(status, "Output array not large enough");
      return;
    }
    return HAL_REV_GetPDHAllChannelCurrents(handle, currents, status);
  }
}

double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalCurrent(handle, status);
  } else {
    return HAL_REV_GetPDHTotalCurrent(handle, status);
  }
}

double HAL_GetPowerDistributionTotalPower(HAL_PowerDistributionHandle handle,
                                          int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalPower(handle, status);
  } else {
    // Not currently supported
    return 0;
  }
}

double HAL_GetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalEnergy(handle, status);
  } else {
    // Not currently supported
    return 0;
  }
}

void HAL_ResetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  if (IsCtre(handle)) {
    HAL_ResetPDPTotalEnergy(handle, status);
  } else {
    // Not supported
  }
}

void HAL_ClearPowerDistributionStickyFaults(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  if (IsCtre(handle)) {
    HAL_ClearPDPStickyFaults(handle, status);
  } else {
    HAL_REV_ClearPDHFaults(handle, status);
  }
}

void HAL_SetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, HAL_Bool enabled, int32_t* status) {
  if (IsCtre(handle)) {
    // No-op on CTRE
    return;
  } else {
    HAL_REV_SetPDHSwitchableChannel(handle, enabled, status);
  }
}

HAL_Bool HAL_GetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  if (IsCtre(handle)) {
    // No-op on CTRE
    return false;
  } else {
    return HAL_REV_GetPDHSwitchableChannelState(handle, status);
  }
}

}  // extern "C"

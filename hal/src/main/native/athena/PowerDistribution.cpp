// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PowerDistribution.h"

#include <cstring>
#include <thread>

#include "CTREPDP.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "REVPDH.h"
#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t moduleNumber, HAL_PowerDistributionType type,
    const char* allocationLocation, int32_t* status) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kAutomatic) {
    if (moduleNumber != HAL_DEFAULT_POWER_DISTRIBUTION_MODULE) {
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(
          status, "Automatic PowerDistributionType must have default module");
      return HAL_kInvalidHandle;
    }

    uint64_t waitTime = hal::GetDSInitializeTime() + 400000;

    // Ensure we have been alive for long enough to receive a few Power packets.
    do {
      uint64_t currentTime = HAL_GetFPGATime(status);
      if (*status != 0) {
        return HAL_kInvalidHandle;
      }
      if (currentTime >= waitTime) {
        break;
      }
      std::this_thread::sleep_for(
          std::chrono::microseconds(waitTime - currentTime));
    } while (true);

    // Try PDP first
    auto pdpHandle = HAL_InitializePDP(0, allocationLocation, status);
    if (pdpHandle != HAL_kInvalidHandle) {
      *status = 0;
      HAL_GetPDPVoltage(pdpHandle, status);
      if (*status == 0 || *status == HAL_CAN_TIMEOUT) {
        return static_cast<HAL_PowerDistributionHandle>(pdpHandle);
      }
      HAL_CleanPDP(pdpHandle);
    }
    *status = 0;
    auto pdhHandle = HAL_InitializeREVPDH(1, allocationLocation, status);
    return static_cast<HAL_PowerDistributionHandle>(pdhHandle);
  }

  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    if (moduleNumber == HAL_DEFAULT_POWER_DISTRIBUTION_MODULE) {
      moduleNumber = 0;
    }
    return static_cast<HAL_PowerDistributionHandle>(
        HAL_InitializePDP(moduleNumber, allocationLocation, status));
  } else {
    if (moduleNumber == HAL_DEFAULT_POWER_DISTRIBUTION_MODULE) {
      moduleNumber = 1;
    }
    return static_cast<HAL_PowerDistributionHandle>(
        HAL_InitializeREVPDH(moduleNumber, allocationLocation, status));
  }
}

#define IsCtre(handle) ::hal::isHandleType(handle, HAL_HandleEnum::CTREPDP)

void HAL_CleanPowerDistribution(HAL_PowerDistributionHandle handle) {
  if (IsCtre(handle)) {
    HAL_CleanPDP(handle);
  } else {
    HAL_FreeREVPDH(handle);
  }
}

int32_t HAL_GetPowerDistributionModuleNumber(HAL_PowerDistributionHandle handle,
                                             int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPModuleNumber(handle, status);
  } else {
    return HAL_GetREVPDHModuleNumber(handle, status);
  }
}

HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel) {
  if (IsCtre(handle)) {
    return HAL_CheckPDPChannel(channel);
  } else {
    return HAL_CheckREVPDHChannelNumber(channel);
  }
}

HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    return HAL_CheckPDPModule(module);
  } else {
    return HAL_CheckREVPDHModuleNumber(module);
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
    return HAL_GetREVPDHVoltage(handle, status);
  }
}

double HAL_GetPowerDistributionChannelCurrent(
    HAL_PowerDistributionHandle handle, int32_t channel, int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPChannelCurrent(handle, channel, status);
  } else {
    return HAL_GetREVPDHChannelCurrent(handle, channel, status);
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
    return HAL_GetREVPDHAllChannelCurrents(handle, currents, status);
  }
}

double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  if (IsCtre(handle)) {
    return HAL_GetPDPTotalCurrent(handle, status);
  } else {
    return HAL_GetREVPDHTotalCurrent(handle, status);
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
    HAL_ClearREVPDHStickyFaults(handle, status);
  }
}

void HAL_SetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, HAL_Bool enabled, int32_t* status) {
  if (IsCtre(handle)) {
    // No-op on CTRE
    return;
  } else {
    HAL_SetREVPDHSwitchableChannel(handle, enabled, status);
  }
}

HAL_Bool HAL_GetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  if (IsCtre(handle)) {
    // No-op on CTRE
    return false;
  } else {
    return HAL_GetREVPDHSwitchableChannelState(handle, status);
  }
}

void HAL_GetPowerDistributionVersion(HAL_PowerDistributionHandle handle,
                                     HAL_PowerDistributionVersion* version,
                                     int32_t* status) {
  if (IsCtre(handle)) {
    std::memset(version, 0, sizeof(*version));
  } else {
    HAL_GetREVPDHVersion(handle, version, status);
  }
}

void HAL_GetPowerDistributionFaults(HAL_PowerDistributionHandle handle,
                                    HAL_PowerDistributionFaults* faults,
                                    int32_t* status) {
  if (IsCtre(handle)) {
    std::memset(faults, 0, sizeof(*faults));
  } else {
    HAL_GetREVPDHFaults(handle, faults, status);
  }
}

void HAL_GetPowerDistributionStickyFaults(
    HAL_PowerDistributionHandle handle,
    HAL_PowerDistributionStickyFaults* stickyFaults, int32_t* status) {
  if (IsCtre(handle)) {
    std::memset(stickyFaults, 0, sizeof(*stickyFaults));
  } else {
    HAL_GetREVPDHStickyFaults(handle, stickyFaults, status);
  }
}

}  // extern "C"

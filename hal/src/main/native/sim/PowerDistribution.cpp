// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PowerDistribution.h"

#include <fmt/format.h>

#include "CANAPIInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/CANAPI.h"
#include "hal/Errors.h"
#include "mockdata/PowerDistributionDataInternal.h"

using namespace hal;

static constexpr HAL_CANManufacturer manufacturer =
    HAL_CANManufacturer::HAL_CAN_Man_CTRE;

static constexpr HAL_CANDeviceType deviceType =
    HAL_CANDeviceType::HAL_CAN_Dev_PowerDistribution;

namespace hal::init {
void InitializePowerDistribution() {}
}  // namespace hal::init

extern "C" {
HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t busId, int32_t module, HAL_PowerDistributionType type,
    const char* allocationLocation, int32_t* status) {
  if (type == HAL_PowerDistributionType_Automatic) {
    if (module != HAL_DEFAULT_POWER_DISTRIBUTION_MODULE) {
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(
          status, "Automatic PowerDistributionType must have default module");
      return HAL_InvalidHandle;
    }

    // TODO Make this not matter
    type = HAL_PowerDistributionType_CTRE;
    module = 0;
  }

  if (!HAL_CheckPowerDistributionModule(module, type)) {
    *status = RESOURCE_OUT_OF_RANGE;
    if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_CTRE) {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for CTRE PDP", 0,
                                       NUM_CTRE_PDP_MODULES - 1, module);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for REV PDH", 1,
                                       NUM_REV_PDH_MODULES, module);
    }
    return HAL_InvalidHandle;
  }
  hal::init::CheckInit();
  SimPowerDistributionData[module].initialized = true;
  auto handle =
      HAL_InitializeCAN(busId, manufacturer, module, deviceType, status);

  if (*status != 0) {
    HAL_CleanCAN(handle);
    return HAL_InvalidHandle;
  }

  return handle;
}

int32_t HAL_GetPowerDistributionModuleNumber(HAL_PowerDistributionHandle handle,
                                             int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0;
  }
  return module;
}

HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type) {
  if (type == HAL_PowerDistributionType::HAL_PowerDistributionType_CTRE) {
    return module < NUM_CTRE_PDP_MODULES && module >= 0;
  } else {
    return module <= NUM_REV_PDH_MODULES && module >= 1;
  }
}

HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel) {
  // TODO make this grab from the handle directly
  if (false) {
    return channel < NUM_CTRE_PDP_CHANNELS && channel >= 0;
  } else {
    return channel < NUM_REV_PDH_CHANNELS && channel >= 0;
  }
}

HAL_PowerDistributionType HAL_GetPowerDistributionType(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  return HAL_PowerDistributionType::HAL_PowerDistributionType_CTRE;
}

int32_t HAL_GetPowerDistributionNumChannels(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  // TODO make this grab from the handle directly
  if (false) {
    return NUM_CTRE_PDP_CHANNELS;
  } else {
    return NUM_REV_PDH_CHANNELS;
  }
}

void HAL_CleanPowerDistribution(HAL_PowerDistributionHandle handle) {
  HAL_CleanCAN(handle);
}

double HAL_GetPowerDistributionTemperature(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }
  return SimPowerDistributionData[module].temperature;
}
double HAL_GetPowerDistributionVoltage(HAL_PowerDistributionHandle handle,
                                       int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }
  return SimPowerDistributionData[module].voltage;
}
double HAL_GetPowerDistributionChannelCurrent(
    HAL_PowerDistributionHandle handle, int32_t channel, int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }
  return SimPowerDistributionData[module].current[channel];
}
void HAL_GetPowerDistributionAllChannelCurrents(
    HAL_PowerDistributionHandle handle, double* currents,
    int32_t currentsLength, int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return;
  }

  auto& data = SimPowerDistributionData[module];
  int toCopy = (std::min)(currentsLength, NUM_PD_SIM_CHANNELS);
  for (int i = 0; i < toCopy; i++) {
    currents[i] = data.current[i];
  }
}
double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }

  double total = 0.0;
  auto& data = SimPowerDistributionData[module];
  for (int i = 0; i < NUM_PD_SIM_CHANNELS; i++) {
    total += data.current[i];
  }
  return total;
}
double HAL_GetPowerDistributionTotalPower(HAL_PowerDistributionHandle handle,
                                          int32_t* status) {
  double voltage = HAL_GetPowerDistributionVoltage(handle, status);
  double current = HAL_GetPowerDistributionTotalCurrent(handle, status);
  return voltage * current;
}
double HAL_GetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  return 0.0;
}
void HAL_ResetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {}
void HAL_ClearPowerDistributionStickyFaults(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {}
void HAL_SetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, HAL_Bool state, int32_t* status) {}

HAL_Bool HAL_GetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  return false;
}

void HAL_GetPowerDistributionVersion(HAL_PowerDistributionHandle handle,
                                     HAL_PowerDistributionVersion* version,
                                     int32_t* status) {}

void HAL_GetPowerDistributionFaults(HAL_PowerDistributionHandle handle,
                                    HAL_PowerDistributionFaults* faults,
                                    int32_t* status) {}

void HAL_GetPowerDistributionStickyFaults(
    HAL_PowerDistributionHandle handle,
    HAL_PowerDistributionStickyFaults* stickyFaults, int32_t* status) {}
}  // extern "C"

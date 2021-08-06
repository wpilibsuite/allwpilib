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
    HAL_CANManufacturer::HAL_CAN_Man_kCTRE;

static constexpr HAL_CANDeviceType deviceType =
    HAL_CANDeviceType::HAL_CAN_Dev_kPowerDistribution;

namespace hal::init {
void InitializePowerDistribution() {}
}  // namespace hal::init

extern "C" {
HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t module, HAL_PowerDistributionType type, int32_t* status) {
  if (!HAL_CheckPowerDistributionModule(module, type)) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(status, fmt::format("Invalid pdp module {}", module));
    return HAL_kInvalidHandle;
  }
  hal::init::CheckInit();
  SimPowerDistributionData[module].initialized = true;
  auto handle = HAL_InitializeCAN(manufacturer, module, deviceType, status);

  if (*status != 0) {
    HAL_CleanCAN(handle);
    return HAL_kInvalidHandle;
  }

  return handle;
}

HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type) {
  return module < kNumPDPModules && module >= 0;
}

HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel) {
  return channel < kNumPDPChannels && channel >= 0;
}

HAL_PowerDistributionType HAL_GetPowerDistributionType(
    HAL_PowerDistributionHandle handle, int32_t* status) {
  return HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE;
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
  for (int i = 0; i < kNumPDPChannels; i++) {
    currents[i] = data.current[i];
  }
}
double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {
  return 0.0;
}
double HAL_GetPowerDistributionTotalPower(HAL_PowerDistributionHandle handle,
                                          int32_t* status) {
  return 0.0;
}
double HAL_GetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {
  return 0.0;
}
void HAL_ResetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status) {}
void HAL_ClearPowerDistributionStickyFaults(HAL_PowerDistributionHandle handle,
                                            int32_t* status) {}
}  // extern "C"

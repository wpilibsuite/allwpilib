/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/PDP.h"

#include "CANAPIInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/CANAPI.h"
#include "hal/handles/IndexedHandleResource.h"
#include "mockdata/PDPDataInternal.h"

using namespace hal;

static constexpr HAL_CANManufacturer manufacturer =
    HAL_CANManufacturer::HAL_CAN_Man_kCTRE;

static constexpr HAL_CANDeviceType deviceType =
    HAL_CANDeviceType::HAL_CAN_Dev_kPowerDistribution;

namespace hal {
namespace init {
void InitializePDP() {}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_PDPHandle HAL_InitializePDP(int32_t module, int32_t* status) {
  if (!HAL_CheckPDPModule(module)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }
  hal::init::CheckInit();
  SimPDPData[module].initialized = true;
  auto handle = HAL_InitializeCAN(manufacturer, module, deviceType, status);

  if (*status != 0) {
    HAL_CleanCAN(handle);
    return HAL_kInvalidHandle;
  }

  return handle;
}

HAL_Bool HAL_CheckPDPModule(int32_t module) {
  return module < kNumPDPModules && module >= 0;
}

HAL_Bool HAL_CheckPDPChannel(int32_t channel) {
  return channel < kNumPDPChannels && channel >= 0;
}

void HAL_CleanPDP(HAL_PDPHandle handle) { HAL_CleanCAN(handle); }

double HAL_GetPDPTemperature(HAL_PDPHandle handle, int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }
  return SimPDPData[module].temperature;
}
double HAL_GetPDPVoltage(HAL_PDPHandle handle, int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }
  return SimPDPData[module].voltage;
}
double HAL_GetPDPChannelCurrent(HAL_PDPHandle handle, int32_t channel,
                                int32_t* status) {
  auto module = hal::can::GetCANModuleFromHandle(handle, status);
  if (*status != 0) {
    return 0.0;
  }
  return SimPDPData[module].current[channel];
}
double HAL_GetPDPTotalCurrent(HAL_PDPHandle handle, int32_t* status) {
  return 0.0;
}
double HAL_GetPDPTotalPower(HAL_PDPHandle handle, int32_t* status) {
  return 0.0;
}
double HAL_GetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status) {
  return 0.0;
}
void HAL_ResetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status) {}
void HAL_ClearPDPStickyFaults(HAL_PDPHandle handle, int32_t* status) {}
}  // extern "C"

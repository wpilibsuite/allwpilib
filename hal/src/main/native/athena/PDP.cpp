/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PDP.h"

#include <memory>

#include "HAL/Errors.h"
#include "HAL/Ports.h"
#include "HAL/cpp/make_unique.h"
#include "PortsInternal.h"
#include "ctre/PDP.h"

using namespace hal;

static std::unique_ptr<PDP> pdp[kNumPDPModules];

static inline bool checkPDPInit(int32_t module, int32_t* status) {
  if (!HAL_CheckPDPModule(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return false;
  }
  if (!pdp[module]) {
    *status = INCOMPATIBLE_STATE;
    return false;
  }
  return true;
}

extern "C" {

void HAL_InitializePDP(int32_t module, int32_t* status) {
  if (!HAL_CheckPDPModule(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return;
  }
  if (!pdp[module]) {
    pdp[module] = std::make_unique<PDP>(module);
  }
}

HAL_Bool HAL_CheckPDPModule(int32_t module) {
  return module < kNumPDPModules && module >= 0;
}

HAL_Bool HAL_CheckPDPChannel(int32_t channel) {
  return channel < kNumPDPChannels && channel >= 0;
}

double HAL_GetPDPTemperature(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return 0;

  double temperature;

  *status = pdp[module]->GetTemperature(temperature);

  return temperature;
}

double HAL_GetPDPVoltage(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return 0;

  double voltage;

  *status = pdp[module]->GetVoltage(voltage);

  return voltage;
}

double HAL_GetPDPChannelCurrent(int32_t module, int32_t channel,
                                int32_t* status) {
  if (!checkPDPInit(module, status)) return 0;

  double current;

  *status = pdp[module]->GetChannelCurrent(channel, current);

  return current;
}

double HAL_GetPDPTotalCurrent(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return 0;

  double current;

  *status = pdp[module]->GetTotalCurrent(current);

  return current;
}

double HAL_GetPDPTotalPower(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return 0;

  double power;

  *status = pdp[module]->GetTotalPower(power);

  return power;
}

double HAL_GetPDPTotalEnergy(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return 0;

  double energy;

  *status = pdp[module]->GetTotalEnergy(energy);

  return energy;
}

void HAL_ResetPDPTotalEnergy(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return;

  *status = pdp[module]->ResetEnergy();
}

void HAL_ClearPDPStickyFaults(int32_t module, int32_t* status) {
  if (!checkPDPInit(module, status)) return;

  *status = pdp[module]->ClearStickyFaults();
}

}  // extern "C"

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PDP.h"

#include "HAL/Ports.h"
#include "PortsInternal.h"
#include "ctre/PDP.h"

using namespace hal;

static PDP* pdp[kNumPDPModules] = {nullptr};

extern "C" {

void HAL_InitializePDP(int32_t module) {
  if (!pdp[module]) {
    pdp[module] = new PDP(module);
  }
}

float HAL_GetPDPTemperature(int32_t module, int32_t* status) {
  double temperature;

  *status = pdp[module]->GetTemperature(temperature);

  return static_cast<float>(temperature);
}

float HAL_GetPDPVoltage(int32_t module, int32_t* status) {
  double voltage;

  *status = pdp[module]->GetVoltage(voltage);

  return static_cast<float>(voltage);
}

float HAL_GetPDPChannelCurrent(int32_t module, int32_t channel,
                               int32_t* status) {
  double current;

  *status = pdp[module]->GetChannelCurrent(channel, current);

  return static_cast<float>(current);
}

float HAL_GetPDPTotalCurrent(int32_t module, int32_t* status) {
  double current;

  *status = pdp[module]->GetTotalCurrent(current);

  return static_cast<float>(current);
}

float HAL_GetPDPTotalPower(int32_t module, int32_t* status) {
  double power;

  *status = pdp[module]->GetTotalPower(power);

  return static_cast<float>(power);
}

float HAL_GetPDPTotalEnergy(int32_t module, int32_t* status) {
  double energy;

  *status = pdp[module]->GetTotalEnergy(energy);

  return static_cast<float>(energy);
}

void HAL_ResetPDPTotalEnergy(int32_t module, int32_t* status) {
  *status = pdp[module]->ResetEnergy();
}

void HAL_ClearPDPStickyFaults(int32_t module, int32_t* status) {
  *status = pdp[module]->ClearStickyFaults();
}

}  // extern "C"

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

void HAL_InitializePDP(uint8_t module) {
  if (!pdp[module]) {
    pdp[module] = new PDP(module);
  }
}

double HAL_GetPDPTemperature(uint8_t module, int32_t* status) {
  double temperature;

  *status = pdp[module]->GetTemperature(temperature);

  return temperature;
}

double HAL_GetPDPVoltage(uint8_t module, int32_t* status) {
  double voltage;

  *status = pdp[module]->GetVoltage(voltage);

  return voltage;
}

double HAL_GetPDPChannelCurrent(uint8_t module, uint8_t channel,
                                int32_t* status) {
  double current;

  *status = pdp[module]->GetChannelCurrent(channel, current);

  return current;
}

double HAL_GetPDPTotalCurrent(uint8_t module, int32_t* status) {
  double current;

  *status = pdp[module]->GetTotalCurrent(current);

  return current;
}

double HAL_GetPDPTotalPower(uint8_t module, int32_t* status) {
  double power;

  *status = pdp[module]->GetTotalPower(power);

  return power;
}

double HAL_GetPDPTotalEnergy(uint8_t module, int32_t* status) {
  double energy;

  *status = pdp[module]->GetTotalEnergy(energy);

  return energy;
}

void HAL_ResetPDPTotalEnergy(uint8_t module, int32_t* status) {
  *status = pdp[module]->ResetEnergy();
}

void HAL_ClearPDPStickyFaults(uint8_t module, int32_t* status) {
  *status = pdp[module]->ClearStickyFaults();
}

}  // extern "C"

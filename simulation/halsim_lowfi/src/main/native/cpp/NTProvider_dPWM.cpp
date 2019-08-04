/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_dPWM.h"

#include <hal/Ports.h>
#include <mockdata/DigitalPWMData.h>

void HALSimNTProviderDigitalPWM::Initialize() {
  InitializeDefault(HAL_GetNumDigitalPWMOutputs(),
                    HALSIM_RegisterDigitalPWMAllCallbacks);
}

void HALSimNTProviderDigitalPWM::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init?").SetBoolean(HALSIM_GetDigitalPWMInitialized(chan));
  table->GetEntry("dio_pin").SetDouble(HALSIM_GetDigitalPWMPin(chan));
  table->GetEntry("duty_cycle").SetDouble(HALSIM_GetDigitalPWMDutyCycle(chan));
}

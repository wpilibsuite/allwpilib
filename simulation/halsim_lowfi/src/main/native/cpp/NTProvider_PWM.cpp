/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_PWM.h"

#include <hal/Ports.h>
#include <mockdata/PWMData.h>

void HALSimNTProviderPWM::Initialize() {
  InitializeDefault(HAL_GetNumPWMChannels(), HALSIM_RegisterPWMAllCallbacks);
}

void HALSimNTProviderPWM::OnCallback(uint32_t chan,
                                     std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init?").SetBoolean(HALSIM_GetPWMInitialized(chan));
  table->GetEntry("speed").SetDouble(HALSIM_GetPWMSpeed(chan));
  table->GetEntry("position").SetDouble(HALSIM_GetPWMPosition(chan));
  table->GetEntry("raw").SetDouble(HALSIM_GetPWMRawValue(chan));
  table->GetEntry("period_scale").SetDouble(HALSIM_GetPWMPeriodScale(chan));
  table->GetEntry("zero_latch?").SetBoolean(HALSIM_GetPWMZeroLatch(chan));
}

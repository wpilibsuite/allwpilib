/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_dPWM.h"

#include <hal/Ports.h>
#include <hal/simulation/DigitalPWMData.h>

namespace wpilibws {

void HALSimWSProviderDigitalPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderDigitalPWM>(
      "dPWM", HAL_GetNumDigitalPWMOutputs(),
      HALSIM_RegisterDigitalPWMAllCallbacks, webRegisterFunc);
}

wpi::json HALSimWSProviderDigitalPWM::OnSimValueChanged(const char* cbName) {
  return {
      {"<init", static_cast<bool>(HALSIM_GetDigitalPWMInitialized(m_channel))},
      {"<dio_pin", HALSIM_GetDigitalPWMPin(m_channel)},
      {"<duty_cycle", HALSIM_GetDigitalPWMDutyCycle(m_channel)},
  };
}

}  // namespace wpilibws

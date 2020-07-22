/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PWM.h"

#include <hal/Ports.h>
#include <hal/simulation/PWMData.h>

namespace wpilibws {

void HALSimWSProviderPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPWM>("PWM", HAL_GetNumPWMChannels(),
                                       HALSIM_RegisterPWMAllCallbacks,
                                       webRegisterFunc);
}

wpi::json HALSimWSProviderPWM::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);

  if (cbType == "Initialized") {
    return {
      {"<init", static_cast<bool>(HALSIM_GetPWMInitialized(m_channel))}
    };
  }
  else if (cbType == "Speed") {
    return {
      {"<speed", HALSIM_GetPWMSpeed(m_channel)}
    };
  }
  else if (cbType == "Position") {
    return {
      {"<position", HALSIM_GetPWMPosition(m_channel)}
    };
  }
  else if (cbType == "RawValue") {
    return {
      {"<raw", HALSIM_GetPWMRawValue(m_channel)}
    };
  }
  else if (cbType == "PeriodScale") {
    return {
      {"<period_scale", HALSIM_GetPWMPeriodScale(m_channel)}
    };
  }
  else if (cbType == "ZeroLatch") {
    return {
      {"<zero_latch", static_cast<bool>(HALSIM_GetPWMZeroLatch(m_channel))}
    };
  }

  return {};
}

}  // namespace wpilibws

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
  return {
      {"<init", (bool)HALSIM_GetPWMInitialized(m_channel)},
      {"<speed", HALSIM_GetPWMSpeed(m_channel)},
      {"<position", HALSIM_GetPWMPosition(m_channel)},
      {"<raw", HALSIM_GetPWMRawValue(m_channel)},
      {"<period_scale", HALSIM_GetPWMPeriodScale(m_channel)},
      {"<zero_latch", (bool)HALSIM_GetPWMZeroLatch(m_channel)},
  };
}

}
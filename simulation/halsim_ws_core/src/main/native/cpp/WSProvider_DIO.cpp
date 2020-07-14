/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_DIO.h"

#include <hal/Ports.h>
#include <hal/simulation/DIOData.h>

namespace wpilibws {

void HALSimWSProviderDIO::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderDIO>("DIO", HAL_GetNumDigitalChannels(),
                                       HALSIM_RegisterDIOAllCallbacks,
                                       webRegisterFunc);
}

wpi::json HALSimWSProviderDIO::OnSimValueChanged(const char* cbName) {
  return {
      {"<init", (bool)HALSIM_GetDIOInitialized(m_channel)},
      {"<>value", (bool)HALSIM_GetDIOValue(m_channel)},
      {"<pulse_length", HALSIM_GetDIOPulseLength(m_channel)},
      {"<input", (bool)HALSIM_GetDIOIsInput(m_channel)},
  };
}

void HALSimWSProviderDIO::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find("<>value")) != json.end()) {
    HALSIM_SetDIOValue(m_channel, (bool)it.value());
  }
}

}

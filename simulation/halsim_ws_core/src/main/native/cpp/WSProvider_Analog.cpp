/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Analog.h"

#include <hal/Ports.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/AnalogOutData.h>

namespace wpilibws {

void HALSimWSProviderAnalogIn::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogIn>("AI", HAL_GetNumAnalogInputs(),
                                            HALSIM_RegisterAnalogInAllCallbacks,
                                            webRegisterFunc);
}

wpi::json HALSimWSProviderAnalogIn::OnSimValueChanged(const char* cbName) {
  return {
      {"<init", static_cast<bool>(HALSIM_GetAnalogInInitialized(m_channel))},
      {"<avg_bits", HALSIM_GetAnalogInAverageBits(m_channel)},
      {"<oversample_bits", HALSIM_GetAnalogInOversampleBits(m_channel)},
      {">voltage", HALSIM_GetAnalogInVoltage(m_channel)},
      {
          "accum",
          {
              {"<init",
               static_cast<bool>(
                   HALSIM_GetAnalogInAccumulatorInitialized(m_channel))},
              {">value", HALSIM_GetAnalogInAccumulatorValue(m_channel)},
              {">count", HALSIM_GetAnalogInAccumulatorCount(m_channel)},
              {"<center", HALSIM_GetAnalogInAccumulatorCenter(m_channel)},
              {"<deadband", HALSIM_GetAnalogInAccumulatorDeadband(m_channel)},
          },
      },
  };
}

void HALSimWSProviderAnalogIn::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">voltage")) != json.end()) {
    HALSIM_SetAnalogInVoltage(m_channel, it.value());
  }
  if ((it = json.find("accum")) != json.end()) {
    auto accum = it.value();
    if ((it = accum.find(">value")) != accum.end()) {
      HALSIM_SetAnalogInAccumulatorValue(m_channel, it.value());
    }
    if ((it = json.find(">count")) != json.end()) {
      HALSIM_SetAnalogInAccumulatorCount(m_channel, it.value());
    }
  }
}

void HALSimWSProviderAnalogOut::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogOut>(
      "AO", HAL_GetNumAnalogOutputs(), HALSIM_RegisterAnalogOutAllCallbacks,
      webRegisterFunc);
}

wpi::json HALSimWSProviderAnalogOut::OnSimValueChanged(const char* cbName) {
  return {{"<init", HALSIM_GetAnalogOutInitialized(m_channel)},
          {"<voltage", HALSIM_GetAnalogOutVoltage(m_channel)}};
}

}  // namespace wpilibws

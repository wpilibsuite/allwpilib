/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Encoder.h"

#include <hal/Ports.h>
#include <hal/simulation/EncoderData.h>

namespace wpilibws {

void HALSimWSProviderEncoder::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderEncoder>("Encoder", HAL_GetNumEncoders(),
                                           HALSIM_RegisterEncoderAllCallbacks,
                                           webRegisterFunc);
}

wpi::json HALSimWSProviderEncoder::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);
  bool sendDiffOnly = (cbType != "");

  wpi::json result;

  if (cbType == "Initialized" || !sendDiffOnly) {
    result["<init"] =
        static_cast<bool>(HALSIM_GetEncoderInitialized(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "Count" || !sendDiffOnly) {
    result[">count"] = HALSIM_GetEncoderCount(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "Period" || !sendDiffOnly) {
    result[">period"] = HALSIM_GetEncoderPeriod(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "Reset" || !sendDiffOnly) {
    result["<reset"] = static_cast<bool>(HALSIM_GetEncoderReset(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "ReverseDirection" || !sendDiffOnly) {
    result["<reverse_direction"] =
        static_cast<bool>(HALSIM_GetEncoderReverseDirection(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "SamplesToAverage" || !sendDiffOnly) {
    result["<samples_to_avg"] = HALSIM_GetEncoderSamplesToAverage(m_channel);
    if (sendDiffOnly) return result;
  }

  return result;
}

void HALSimWSProviderEncoder::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">count")) != json.end()) {
    HALSIM_SetEncoderCount(m_channel, it.value());
  }
  if ((it = json.find(">period")) != json.end()) {
    HALSIM_SetEncoderPeriod(m_channel, it.value());
  }
}

}  // namespace wpilibws

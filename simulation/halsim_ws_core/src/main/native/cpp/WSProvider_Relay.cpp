/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Relay.h"

#include <hal/Ports.h>
#include <hal/simulation/RelayData.h>

namespace wpilibws {
void HALSimWSProviderRelay::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderRelay>("Relay", HAL_GetNumRelayHeaders(),
                                         HALSIM_RegisterRelayAllCallbacks,
                                         webRegisterFunc);
}

wpi::json HALSimWSProviderRelay::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);
  bool sendDiffOnly = (cbType != "");

  wpi::json result;

  if (cbType == "InitializedForward" || !sendDiffOnly) {
    result["<init_fwd"] =
        static_cast<bool>(HALSIM_GetRelayInitializedForward(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "InitializedReverse" || !sendDiffOnly) {
    result["<init_rev"] =
        static_cast<bool>(HALSIM_GetRelayInitializedReverse(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "Forward" || !sendDiffOnly) {
    result["<fwd"] = static_cast<bool>(HALSIM_GetRelayForward(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "Reverse" || !sendDiffOnly) {
    result["<rev"] = static_cast<bool>(HALSIM_GetRelayReverse(m_channel));
    if (sendDiffOnly) return result;
  }

  return result;
}

}  // namespace wpilibws

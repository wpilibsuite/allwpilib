/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_Relay.h"

#include <hal/Ports.h>
#include <mockdata/RelayData.h>

void HALSimNTProviderRelay::Initialize() {
  InitializeDefault(HAL_GetNumRelayHeaders(), HALSIM_RegisterRelayAllCallbacks);
}

void HALSimNTProviderRelay::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init_fwd?")
      .SetBoolean(HALSIM_GetRelayInitializedForward(chan));
  table->GetEntry("init_rvs?")
      .SetBoolean(HALSIM_GetRelayInitializedReverse(chan));
  table->GetEntry("fwd?").SetBoolean(HALSIM_GetRelayForward(chan));
  table->GetEntry("rvs?").SetBoolean(HALSIM_GetRelayReverse(chan));
}

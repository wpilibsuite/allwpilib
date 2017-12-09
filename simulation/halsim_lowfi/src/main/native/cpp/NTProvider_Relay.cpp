/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <HAL/Ports.h>
#include <MockData/RelayData.h>

#include <NTProvider_Relay.h>

void HALSimNTProviderRelay::Initialize() {
    // Relay channels are double counted (one for fwd, one for rvs)
    InitializeDefault(HAL_GetNumRelayChannels() / 2, HALSIM_RegisterRelayAllCallbacks);
}

void HALSimNTProviderRelay::OnCallback(uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
    table->GetEntry("init_fwd?").SetBoolean(HALSIM_GetRelayInitializedForward(chan));
    table->GetEntry("init_rvs?").SetBoolean(HALSIM_GetRelayInitializedReverse(chan));
    table->GetEntry("fwd?").SetBoolean(HALSIM_GetRelayForward(chan));
    table->GetEntry("rvs?").SetBoolean(HALSIM_GetRelayReverse(chan));
}
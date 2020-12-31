// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_Relay.h"

#include <hal/Ports.h>
#include <hal/simulation/RelayData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterRelay##halsim##Callback(                                \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderRelay*>(param)->ProcessHalCallback(  \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)

namespace wpilibws {
void HALSimWSProviderRelay::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderRelay>("Relay", HAL_GetNumRelayHeaders(),
                                         webRegisterFunc);
}

HALSimWSProviderRelay::~HALSimWSProviderRelay() {
  DoCancelCallbacks();
}

void HALSimWSProviderRelay::RegisterCallbacks() {
  m_initFwdCbKey = REGISTER(InitializedForward, "<init_fwd", bool, boolean);
  m_initRevCbKey = REGISTER(InitializedReverse, "<init_rev", bool, boolean);
  m_fwdCbKey = REGISTER(Forward, "<fwd", bool, boolean);
  m_revCbKey = REGISTER(Reverse, "<rev", bool, boolean);
}

void HALSimWSProviderRelay::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderRelay::DoCancelCallbacks() {
  HALSIM_CancelRelayInitializedForwardCallback(m_channel, m_initFwdCbKey);
  HALSIM_CancelRelayInitializedReverseCallback(m_channel, m_initRevCbKey);
  HALSIM_CancelRelayForwardCallback(m_channel, m_fwdCbKey);
  HALSIM_CancelRelayReverseCallback(m_channel, m_revCbKey);

  m_initFwdCbKey = 0;
  m_initRevCbKey = 0;
  m_fwdCbKey = 0;
  m_revCbKey = 0;
}

}  // namespace wpilibws

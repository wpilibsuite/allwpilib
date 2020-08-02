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
                                         webRegisterFunc);
}

void HALSimWSProviderRelay::RegisterCallbacks() {
  m_initFwdCbKey = HALSIM_RegisterRelayInitializedForwardCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRelay*>(param)->ProcessHalCallback(
            {{"<init_fwd", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_initRevCbKey = HALSIM_RegisterRelayInitializedReverseCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRelay*>(param)->ProcessHalCallback(
            {{"<init_rev", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_fwdCbKey = HALSIM_RegisterRelayForwardCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRelay*>(param)->ProcessHalCallback(
            {{"<fwd", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_revCbKey = HALSIM_RegisterRelayReverseCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRelay*>(param)->ProcessHalCallback(
            {{"<rev", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);
}

void HALSimWSProviderRelay::CancelCallbacks() {
  HALSIM_CancelRelayInitializedForwardCallback(m_channel, m_initFwdCbKey);
  HALSIM_CancelRelayInitializedReverseCallback(m_channel, m_initRevCbKey);
  HALSIM_CancelRelayForwardCallback(m_channel, m_fwdCbKey);
  HALSIM_CancelRelayReverseCallback(m_channel, m_revCbKey);
}

}  // namespace wpilibws

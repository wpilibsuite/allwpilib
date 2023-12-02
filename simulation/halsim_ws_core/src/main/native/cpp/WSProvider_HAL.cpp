// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_HAL.h"

#include <algorithm>
#include <atomic>
#include <string_view>

#include <hal/Extensions.h>
#include <hal/HAL.h>
#include <hal/Ports.h>
#include <hal/simulation/MockHooks.h>
#include <wpi/raw_ostream.h>

namespace wpilibws {

void HALSimWSProviderHAL::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateSingleProvider<HALSimWSProviderHAL>("HAL", webRegisterFunc);
}

HALSimWSProviderHAL::~HALSimWSProviderHAL() {
  DoCancelCallbacks();
}

void HALSimWSProviderHAL::RegisterCallbacks() {
  m_simPeriodicBeforeCbKey = HALSIM_RegisterSimPeriodicBeforeCallback(
      [](void* param) {
        static_cast<HALSimWSProviderHAL*>(param)->ProcessHalCallback(
            {{">sim_periodic_before", true}});
      },
      this);

  m_simPeriodicAfterCbKey = HALSIM_RegisterSimPeriodicAfterCallback(
      [](void* param) {
        static_cast<HALSimWSProviderHAL*>(param)->ProcessHalCallback(
            {{">sim_periodic_after", true}});
      },
      this);
}

void HALSimWSProviderHAL::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderHAL::DoCancelCallbacks() {
  HALSIM_CancelSimPeriodicBeforeCallback(m_simPeriodicBeforeCbKey);
  HALSIM_CancelSimPeriodicAfterCallback(m_simPeriodicAfterCbKey);

  m_simPeriodicBeforeCbKey = 0;
  m_simPeriodicAfterCbKey = 0;
}

void HALSimWSProviderHAL::OnNetValueChanged(const wpi::json& json) {
  // no-op. This is all one way
}

}  // namespace wpilibws

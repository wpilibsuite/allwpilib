/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_dPWM.h"

#include <hal/Ports.h>
#include <hal/simulation/DigitalPWMData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                             \
  HALSIM_RegisterDigitalPWM##halsim##Callback(                               \
      m_channel,                                                             \
      [](const char* name, void* param, const struct HAL_Value* value) {     \
        static_cast<HALSimWSProviderDigitalPWM*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});        \
      },                                                                     \
      this, true)

namespace wpilibws {

void HALSimWSProviderDigitalPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderDigitalPWM>(
      "dPWM", HAL_GetNumDigitalPWMOutputs(), webRegisterFunc);
}

HALSimWSProviderDigitalPWM::~HALSimWSProviderDigitalPWM() {
  DoCancelCallbacks();
}

void HALSimWSProviderDigitalPWM::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<init", bool, boolean);
  m_dutyCycleCbKey = REGISTER(DutyCycle, "<duty_cycle", double, double);
  m_pinCbKey = REGISTER(Pin, "<dio_pin", int32_t, int);
}

void HALSimWSProviderDigitalPWM::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderDigitalPWM::DoCancelCallbacks() {
  HALSIM_CancelDigitalPWMInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelDigitalPWMDutyCycleCallback(m_channel, m_dutyCycleCbKey);
  HALSIM_CancelDigitalPWMPinCallback(m_channel, m_pinCbKey);

  m_initCbKey = 0;
  m_dutyCycleCbKey = 0;
  m_pinCbKey = 0;
}

}  // namespace wpilibws

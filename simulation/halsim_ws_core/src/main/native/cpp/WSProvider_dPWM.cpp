/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_dPWM.h"

#include <hal/Ports.h>
#include <hal/simulation/DigitalPWMData.h>

namespace wpilibws {

void HALSimWSProviderDigitalPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderDigitalPWM>(
      "dPWM", HAL_GetNumDigitalPWMOutputs(), webRegisterFunc);
}

void HALSimWSProviderDigitalPWM::RegisterCallbacks() {
  m_initCbKey = HALSIM_RegisterDigitalPWMInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDigitalPWM*>(param)->ProcessHalCallback(
            {{"<init", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_dutyCycleCbKey = HALSIM_RegisterDigitalPWMDutyCycleCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDigitalPWM*>(param)->ProcessHalCallback(
            {{"<duty_cycle", value->data.v_double}});
      },
      this, true);

  m_pinCbKey = HALSIM_RegisterDigitalPWMPinCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDigitalPWM*>(param)->ProcessHalCallback(
            {{"<dio_pin", value->data.v_int}});
      },
      this, true);
}

void HALSimWSProviderDigitalPWM::CancelCallbacks() {
  HALSIM_CancelDigitalPWMInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelDigitalPWMDutyCycleCallback(m_channel, m_dutyCycleCbKey);
  HALSIM_CancelDigitalPWMPinCallback(m_channel, m_pinCbKey);
}

}  // namespace wpilibws

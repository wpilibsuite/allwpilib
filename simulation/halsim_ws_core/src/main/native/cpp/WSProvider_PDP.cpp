/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PDP.h"

#include <hal/Ports.h>
#include <hal/simulation/PDPData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterPDP##halsim##Callback(                                  \
      m_index,                                                           \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderPDP*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)
namespace wpilibws {
void HALSimWSProviderPDP::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateSingleProvider<HALSimWSProviderPDP>("PDP", webRegisterFunc);
}

HALSimWSProviderPDP::~HALSimWSProviderPDP() { DoCancelCallbacks(); }

void HALSimWSProviderPDP::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<initialized", bool, boolean);
  m_temperatureCbKey = REGISTER(Temperature, "<temperature", double, double);
  m_voltageCbKey = REGISTER(Voltage, "<voltage", double, double);
  // TODO: How to handle channel current? Provider for each channel?
  // m_currentCbKey = REGISTER(Current, "<current", double, double);
}

void HALSimWSProviderPDP::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderPDP::DoCancelCallbacks() {
  HALSIM_CancelPDPInitializedCallback(m_index, m_initCbKey);
  HALSIM_CancelPDPTemperatureCallback(m_index, m_temperatureCbKey);
  HALSIM_CancelPDPVoltageCallback(m_index, m_voltageCbKey);

  m_initCbKey = 0;
  m_temperatureCbKey = 0;
  m_voltageCbKey = 0;
}
}  // namespace wpilibws

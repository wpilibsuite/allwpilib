/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_AnalogGyro.h"

#include <hal/Ports.h>
#include <hal/simulation/AnalogGyroData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                             \
  HALSIM_RegisterAnalogGyro##halsim##Callback(                               \
      m_channel,                                                             \
      [](const char* name, void* param, const struct HAL_Value* value) {     \
        static_cast<HALSimWSProviderAnalogGyro*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});        \
      },                                                                     \
      this, true)
namespace wpilibws {
void HALSimWSProviderAnalogGyro::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogGyro>("AnalogGyro", 1, webRegisterFunc);
}

HALSimWSProviderAnalogGyro::~HALSimWSProviderAnalogGyro() {
  DoCancelCallbacks();
}

void HALSimWSProviderAnalogGyro::RegisterCallbacks() {
  m_angleCbKey = REGISTER(Angle, "<angle", double, double);
  m_rateCbKey = REGISTER(Rate, "<rate", double, double);
  m_initCbKey = REGISTER(Initialized, "<initialized", bool, boolean);
}

void HALSimWSProviderAnalogGyro::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderAnalogGyro::DoCancelCallbacks() {
  HALSIM_CancelAnalogGyroAngleCallback(m_channel, m_angleCbKey);
  HALSIM_CancelAnalogGyroRateCallback(m_channel, m_rateCbKey);
  HALSIM_CancelAnalogGyroInitializedCallback(m_channel, m_initCbKey);

  m_angleCbKey = 0;
  m_rateCbKey = 0;
  m_initCbKey = 0;
}
}  // namespace wpilibws

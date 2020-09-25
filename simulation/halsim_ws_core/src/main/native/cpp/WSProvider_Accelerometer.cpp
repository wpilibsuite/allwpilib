/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Accelerometer.h"

#include <hal/Ports.h>
#include <hal/simulation/AccelerometerData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterAccelerometer##halsim##Callback(                         \
      m_channel,                                                          \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderAccelerometer*>(param)                \
            ->ProcessHalCallback(                                         \
                {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}); \
      },                                                                  \
      this, true)
namespace wpilibws {
void HALSimWSProviderAccelerometer::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAccelerometer>("Accelerometer", 1,
                                                 webRegisterFunc);
}

HALSimWSProviderAccelerometer::~HALSimWSProviderAccelerometer() {
  DoCancelCallbacks();
}

void HALSimWSProviderAccelerometer::RegisterCallbacks() {
  m_activeCbKey = REGISTER(Active, "<active", bool, boolean);
  m_rangeCbKey = REGISTER(Range, "<range", int32_t, int);
  m_xCbKey = REGISTER(X, "<x", int32_t, int);
  m_yCbKey = REGISTER(Y, "<y", int32_t, int);
  m_zCbKey = REGISTER(Z, "<z", int32_t, int);
}

void HALSimWSProviderAccelerometer::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderAccelerometer::DoCancelCallbacks() {
  HALSIM_CancelAccelerometerActiveCallback(m_channel, m_activeCbKey);
  HALSIM_CancelAccelerometerRangeCallback(m_channel, m_rangeCbKey);
  HALSIM_CancelAccelerometerXCallback(m_channel, m_xCbKey);
  HALSIM_CancelAccelerometerYCallback(m_channel, m_yCbKey);
  HALSIM_CancelAccelerometerZCallback(m_channel, m_zCbKey);

  m_activeCbKey = 0;
  m_rangeCbKey = 0;
  m_xCbKey = 0;
  m_yCbKey = 0;
  m_zCbKey = 0;
}
}  // namespace wpilibws

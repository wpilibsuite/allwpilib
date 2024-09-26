// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_BuiltInAccelerometer.h"

#include <memory>

#include <hal/Ports.h>
#include <hal/simulation/AccelerometerData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterAccelerometer##halsim##Callback(                         \
      0,                                                                  \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderBuiltInAccelerometer*>(param)         \
            ->ProcessHalCallback(                                         \
                {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}); \
      },                                                                  \
      this, true)
namespace wpilibws {
HALSimWSProviderBuiltInAccelerometer::HALSimWSProviderBuiltInAccelerometer()
    : HALSimWSHalProvider("Accel/BuiltInAccel", "Accel") {
  m_deviceId = "BuiltInAccel";
}

void HALSimWSProviderBuiltInAccelerometer::Initialize(
    WSRegisterFunc webRegisterFunc) {
  webRegisterFunc("Accel/BuiltInAccel",
                  std::make_unique<HALSimWSProviderBuiltInAccelerometer>());
}

HALSimWSProviderBuiltInAccelerometer::~HALSimWSProviderBuiltInAccelerometer() {
  DoCancelCallbacks();
}

void HALSimWSProviderBuiltInAccelerometer::RegisterCallbacks() {
  m_activeCbKey = REGISTER(Active, "<init", bool, boolean);
  m_rangeCbKey = HALSIM_RegisterAccelerometerRangeCallback(
      0,
      [](const char* name, void* param, const struct HAL_Value* value) {
        double range;
        switch (value->data.v_int) {
          case 0:
            range = 2;
            break;
          case 1:
            range = 4;
            break;
          case 2:
          default:
            range = 8;
            break;
        }
        static_cast<HALSimWSProviderBuiltInAccelerometer*>(param)
            ->ProcessHalCallback({{"<range", range}});
      },
      this, true);
  m_xCbKey = REGISTER(X, ">x", double, double);
  m_yCbKey = REGISTER(Y, ">y", double, double);
  m_zCbKey = REGISTER(Z, ">z", double, double);
}

void HALSimWSProviderBuiltInAccelerometer::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderBuiltInAccelerometer::DoCancelCallbacks() {
  HALSIM_CancelAccelerometerActiveCallback(0, m_activeCbKey);
  HALSIM_CancelAccelerometerRangeCallback(0, m_rangeCbKey);
  HALSIM_CancelAccelerometerXCallback(0, m_xCbKey);
  HALSIM_CancelAccelerometerYCallback(0, m_yCbKey);
  HALSIM_CancelAccelerometerZCallback(0, m_zCbKey);

  m_activeCbKey = 0;
  m_rangeCbKey = 0;
  m_xCbKey = 0;
  m_yCbKey = 0;
  m_zCbKey = 0;
}

void HALSimWSProviderBuiltInAccelerometer::OnNetValueChanged(
    const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">x")) != json.end()) {
    HALSIM_SetAccelerometerX(0, it.value());
  }
  if ((it = json.find(">y")) != json.end()) {
    HALSIM_SetAccelerometerY(0, it.value());
  }
  if ((it = json.find(">z")) != json.end()) {
    HALSIM_SetAccelerometerZ(0, it.value());
  }
}

}  // namespace wpilibws

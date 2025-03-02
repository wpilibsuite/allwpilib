// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_RoboRIO.h"

#include <hal/Ports.h>
#include <hal/simulation/RoboRioData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterRoboRio##halsim##Callback(                               \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});     \
      },                                                                  \
      this, true)

namespace wpilibws {

void HALSimWSProviderRoboRIO::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateSingleProvider<HALSimWSProviderRoboRIO>("RoboRIO", webRegisterFunc);
}

HALSimWSProviderRoboRIO::~HALSimWSProviderRoboRIO() {
  DoCancelCallbacks();
}

void HALSimWSProviderRoboRIO::RegisterCallbacks() {
  m_vinVoltageCbKey = REGISTER(VInVoltage, ">vin_voltage", double, double);
  m_vinCurrentCbKey = REGISTER(VInCurrent, ">vin_current", double, double);

  m_3v3VoltageCbKey = REGISTER(UserVoltage3V3, ">3v3_voltage", double, double);
  m_3v3CurrentCbKey = REGISTER(UserCurrent3V3, ">3v3_current", double, double);
  m_3v3ActiveCbKey = REGISTER(UserActive3V3, ">3v3_active", bool, boolean);
  m_3v3FaultsCbKey = REGISTER(UserFaults3V3, ">3v3_faults", int32_t, int);
}

void HALSimWSProviderRoboRIO::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderRoboRIO::DoCancelCallbacks() {
  HALSIM_CancelRoboRioVInVoltageCallback(m_vinVoltageCbKey);
  HALSIM_CancelRoboRioVInCurrentCallback(m_vinCurrentCbKey);

  HALSIM_CancelRoboRioUserVoltage3V3Callback(m_3v3VoltageCbKey);
  HALSIM_CancelRoboRioUserCurrent3V3Callback(m_3v3CurrentCbKey);
  HALSIM_CancelRoboRioUserActive3V3Callback(m_3v3ActiveCbKey);
  HALSIM_CancelRoboRioUserFaults3V3Callback(m_3v3FaultsCbKey);

  m_vinVoltageCbKey = 0;
  m_vinCurrentCbKey = 0;
  m_3v3ActiveCbKey = 0;
  m_3v3CurrentCbKey = 0;
  m_3v3FaultsCbKey = 0;
  m_3v3VoltageCbKey = 0;
}

void HALSimWSProviderRoboRIO::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;

  if ((it = json.find(">vin_voltage")) != json.end()) {
    HALSIM_SetRoboRioVInVoltage(it.value());
  }
  if ((it = json.find(">vin_current")) != json.end()) {
    HALSIM_SetRoboRioVInCurrent(it.value());
  }

  if ((it = json.find(">3v3_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage3V3(it.value());
  }
  if ((it = json.find(">3v3_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent3V3(it.value());
  }
  if ((it = json.find(">3v3_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive3V3(static_cast<bool>(it.value()));
  }
  if ((it = json.find(">3v3_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults3V3(it.value());
  }
}

}  // namespace wpilibws

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSProvider_RoboRIO.hpp"

#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/RoboRioData.h"

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterRoboRio##halsim##Callback(                               \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback( \
            wpi::util::json::object(                                      \
                jsonid, static_cast<ctype>(value->data.v_##haltype)));    \
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

  HALSIM_CancelRoboRioUserVoltage3V3Callback(m_3v3VoltageCbKey);
  HALSIM_CancelRoboRioUserCurrent3V3Callback(m_3v3CurrentCbKey);
  HALSIM_CancelRoboRioUserActive3V3Callback(m_3v3ActiveCbKey);
  HALSIM_CancelRoboRioUserFaults3V3Callback(m_3v3FaultsCbKey);

  m_vinVoltageCbKey = 0;
  m_3v3ActiveCbKey = 0;
  m_3v3CurrentCbKey = 0;
  m_3v3FaultsCbKey = 0;
  m_3v3VoltageCbKey = 0;
}

void HALSimWSProviderRoboRIO::OnNetValueChanged(const wpi::util::json& json) {
  if (auto val = json.lookup(">vin_voltage"); val && val->is_number()) {
    HALSIM_SetRoboRioVInVoltage(val->get_number());
  }
  if (auto val = json.lookup(">3v3_voltage"); val && val->is_number()) {
    HALSIM_SetRoboRioUserVoltage3V3(val->get_number());
  }
  if (auto val = json.lookup(">3v3_current"); val && val->is_number()) {
    HALSIM_SetRoboRioUserCurrent3V3(val->get_number());
  }
  if (auto val = json.lookup(">3v3_active"); val && val->is_bool()) {
    HALSIM_SetRoboRioUserActive3V3(val->get_bool());
  }
  if (auto val = json.lookup(">3v3_faults"); val && val->is_number()) {
    HALSIM_SetRoboRioUserFaults3V3(val->get_number());
  }
}

}  // namespace wpilibws

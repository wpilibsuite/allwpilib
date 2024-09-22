// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_DriverStation.h"

#include <atomic>
#include <string>
#include <string_view>

#include <hal/DriverStation.h>
#include <hal/Extensions.h>
#include <hal/Ports.h>
#include <hal/simulation/DriverStationData.h>
#include <wpi/raw_ostream.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterDriverStation##halsim##Callback(                         \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderDriverStation*>(param)                \
            ->ProcessHalCallback(                                         \
                {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}); \
      },                                                                  \
      this, true)

namespace wpilibws {

std::atomic<bool>* gDSSocketConnected{nullptr};

void HALSimWSProviderDriverStation::Initialize(WSRegisterFunc webRegisterFunc) {
  static bool registered = false;
  if (!registered) {
    registered = true;
    HAL_RegisterExtensionListener(
        nullptr, [](void*, const char* name, void* data) {
          if (std::string_view{name} == "ds_socket") {
            gDSSocketConnected = static_cast<std::atomic<bool>*>(data);
          }
        });
  }
  CreateSingleProvider<HALSimWSProviderDriverStation>("DriverStation",
                                                      webRegisterFunc);
}

HALSimWSProviderDriverStation::~HALSimWSProviderDriverStation() {
  DoCancelCallbacks();
}

void HALSimWSProviderDriverStation::RegisterCallbacks() {
  m_enabledCbKey = REGISTER(Enabled, ">enabled", bool, boolean);
  m_autonomousCbKey = REGISTER(Autonomous, ">autonomous", bool, boolean);
  m_testCbKey = REGISTER(Test, ">test", bool, boolean);
  m_estopCbKey = REGISTER(EStop, ">estop", bool, boolean);
  m_fmsCbKey = REGISTER(FmsAttached, ">fms", bool, boolean);
  m_dsCbKey = REGISTER(DsAttached, ">ds", bool, boolean);

  // Special case for new data, since the HAL_Value is empty
  m_newDataCbKey = HALSIM_RegisterDriverStationNewDataCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDriverStation*>(param)->ProcessHalCallback(
            {{">new_data", true}});
      },
      this, true);

  m_allianceCbKey = HALSIM_RegisterDriverStationAllianceStationIdCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        std::string station;
        switch (static_cast<HAL_AllianceStationID>(value->data.v_enum)) {
          case HAL_AllianceStationID_kRed1:
            station = "red1";
            break;
          case HAL_AllianceStationID_kBlue1:
            station = "blue1";
            break;
          case HAL_AllianceStationID_kRed2:
            station = "red2";
            break;
          case HAL_AllianceStationID_kBlue2:
            station = "blue2";
            break;
          case HAL_AllianceStationID_kRed3:
            station = "red3";
            break;
          case HAL_AllianceStationID_kBlue3:
            station = "blue3";
            break;
          case HAL_AllianceStationID_kUnknown:
            station = "unknown";
            break;
        }
        static_cast<HALSimWSProviderDriverStation*>(param)->ProcessHalCallback(
            {{">station", station}});
      },
      this, true);

  m_matchTimeCbKey = REGISTER(MatchTime, ">match_time", double, double);
}

void HALSimWSProviderDriverStation::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderDriverStation::DoCancelCallbacks() {
  HALSIM_CancelDriverStationEnabledCallback(m_enabledCbKey);
  HALSIM_CancelDriverStationAutonomousCallback(m_autonomousCbKey);
  HALSIM_CancelDriverStationTestCallback(m_testCbKey);
  HALSIM_CancelDriverStationEStopCallback(m_estopCbKey);
  HALSIM_CancelDriverStationFmsAttachedCallback(m_fmsCbKey);
  HALSIM_CancelDriverStationDsAttachedCallback(m_dsCbKey);
  HALSIM_CancelDriverStationNewDataCallback(m_newDataCbKey);
  HALSIM_CancelDriverStationAllianceStationIdCallback(m_allianceCbKey);
  HALSIM_CancelDriverStationMatchTimeCallback(m_matchTimeCbKey);

  m_enabledCbKey = 0;
  m_autonomousCbKey = 0;
  m_testCbKey = 0;
  m_estopCbKey = 0;
  m_fmsCbKey = 0;
  m_dsCbKey = 0;
  m_newDataCbKey = 0;
  m_allianceCbKey = 0;
  m_matchTimeCbKey = 0;
}

void HALSimWSProviderDriverStation::OnNetValueChanged(const wpi::json& json) {
  // ignore if DS connected
  if (gDSSocketConnected && *gDSSocketConnected) {
    return;
  }

  wpi::json::const_iterator it;
  if ((it = json.find(">enabled")) != json.end()) {
    HALSIM_SetDriverStationEnabled(it.value());
  }
  if ((it = json.find(">autonomous")) != json.end()) {
    HALSIM_SetDriverStationAutonomous(it.value());
  }
  if ((it = json.find(">test")) != json.end()) {
    HALSIM_SetDriverStationTest(it.value());
  }
  if ((it = json.find(">estop")) != json.end()) {
    HALSIM_SetDriverStationEStop(it.value());
  }
  if ((it = json.find(">fms")) != json.end()) {
    HALSIM_SetDriverStationFmsAttached(it.value());
  }
  if ((it = json.find(">ds")) != json.end()) {
    HALSIM_SetDriverStationDsAttached(it.value());
  }

  if ((it = json.find(">station")) != json.end()) {
    auto& station = it.value().get_ref<const std::string&>();
    if (station == "red1") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kRed1);
    } else if (station == "red2") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kRed2);
    } else if (station == "red3") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kRed3);
    } else if (station == "blue1") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kBlue1);
    } else if (station == "blue2") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kBlue2);
    } else if (station == "blue3") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kBlue3);
    }
  }

  if ((it = json.find(">match_time")) != json.end()) {
    HALSIM_SetDriverStationMatchTime(it.value());
  }
  if ((it = json.find(">game_data")) != json.end()) {
    std::string message = it.value().get_ref<const std::string&>();
    auto str = wpi::make_string(message);
    HALSIM_SetGameSpecificMessage(&str);
  }

  // Only notify usercode if we get the new data message
  if ((it = json.find(">new_data")) != json.end()) {
    HALSIM_NotifyDriverStationNewData();
  }
}

}  // namespace wpilibws

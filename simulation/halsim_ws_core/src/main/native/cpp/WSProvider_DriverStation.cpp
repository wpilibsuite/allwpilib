/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_DriverStation.h"

#include <algorithm>

#include <hal/DriverStation.h>
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

void HALSimWSProviderDriverStation::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateSingleProvider<HALSimWSProviderDriverStation>("DriverStation",
                                                      webRegisterFunc);
}

void HALSimWSProviderDriverStation::RegisterCallbacks() {
  m_enabledCbKey = REGISTER(Enabled, ">enabled", bool, boolean);
  m_autonomousCbKey = REGISTER(Autonomous, ">autonomous", bool, boolean);
  m_testCbKey = REGISTER(Test, ">test", bool, boolean);
  m_estopCbKey = REGISTER(EStop, ">estop", bool, boolean);
  m_fmsCbKey = REGISTER(FmsAttached, ">fms", bool, boolean);
  m_dsCbKey = REGISTER(DsAttached, ">ds", bool, boolean);

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
        }
        static_cast<HALSimWSProviderDriverStation*>(param)->ProcessHalCallback(
            {{">station", station}});
      },
      this, true);

  m_matchTimeCbKey = REGISTER(MatchTime, "<match_time", double, double);
}

void HALSimWSProviderDriverStation::CancelCallbacks() {
  HALSIM_CancelDriverStationEnabledCallback(m_enabledCbKey);
  HALSIM_CancelDriverStationAutonomousCallback(m_autonomousCbKey);
  HALSIM_CancelDriverStationTestCallback(m_testCbKey);
  HALSIM_CancelDriverStationEStopCallback(m_estopCbKey);
  HALSIM_CancelDriverStationFmsAttachedCallback(m_fmsCbKey);
  HALSIM_CancelDriverStationDsAttachedCallback(m_dsCbKey);
  HALSIM_CancelDriverStationAllianceStationIdCallback(m_allianceCbKey);
  HALSIM_CancelDriverStationMatchTimeCallback(m_matchTimeCbKey);
}

void HALSimWSProviderDriverStation::OnNetValueChanged(const wpi::json& json) {
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

  if ((it = json.find("joysticks")) != json.end()) {
    auto joysticks = it.value();
    for (auto jit = joysticks.cbegin(); jit != joysticks.cend(); ++jit) {
      auto stick = jit.value();

      int num;
      try {
        num = std::stoi(jit.key());
      } catch (const std::invalid_argument& err) {
        wpi::errs() << "Error converting Joystick number. Skipping. "
                    << err.what() << "\n";
        continue;
      }

      if ((it = stick.find(">axes")) != stick.end()) {
        HAL_JoystickAxes axes{};
        axes.count = std::min(it.value().size(),
                              (wpi::json::size_type)HAL_kMaxJoystickAxes);
        for (int i = 0; i < axes.count; ++i) {
          axes.axes[i] = it.value()[i];
        }
        HALSIM_SetJoystickAxes(num, &axes);
      }

      if ((it = stick.find(">buttons")) != stick.end()) {
        HAL_JoystickButtons buttons{};
        buttons.count = std::min(it.value().size(), (wpi::json::size_type)32);
        for (int i = 0; i < buttons.count; ++i) {
          if (it.value()[i]) {
            buttons.buttons |= 1 << (i - 1);
          }
        }
        HALSIM_SetJoystickButtons(num, &buttons);
      }

      if ((it = stick.find(">povs")) != stick.end()) {
        HAL_JoystickPOVs povs{};
        povs.count = std::min(it.value().size(),
                              (wpi::json::size_type)HAL_kMaxJoystickPOVs);
        for (int i = 0; i < povs.count; ++i) {
          povs.povs[i] = it.value()[i];
        }
        HALSIM_SetJoystickPOVs(num, &povs);
      }
    }
  }

  HALSIM_NotifyDriverStationNewData();
}

}  // namespace wpilibws

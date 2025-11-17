// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/SimCallbackRegistry.h"
#include "wpi/hal/simulation/SimDataValue.h"
#include "wpi/util/spinlock.hpp"

namespace wpi::hal {

class DriverStationData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Enabled)
  HAL_SIMDATAVALUE_DEFINE_NAME(Autonomous)
  HAL_SIMDATAVALUE_DEFINE_NAME(Test)
  HAL_SIMDATAVALUE_DEFINE_NAME(EStop)
  HAL_SIMDATAVALUE_DEFINE_NAME(FmsAttached)
  HAL_SIMDATAVALUE_DEFINE_NAME(DsAttached)
  HAL_SIMDATAVALUE_DEFINE_NAME(AllianceStationId)
  HAL_SIMDATAVALUE_DEFINE_NAME(MatchTime)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickAxes)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickPOVs)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickButtons)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickDescriptor)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickLeds)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickRumbles)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(MatchInfo)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(NewData)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE HAL_Value
  MakeAllianceStationIdValue(HAL_AllianceStationID value) {
    return HAL_MakeEnum(value);
  }

 public:
  DriverStationData();
  void ResetData();

  int32_t RegisterJoystickAxesCallback(int32_t joystickNum,
                                       HAL_JoystickAxesCallback callback,
                                       void* param, HAL_Bool initialNotify);
  void CancelJoystickAxesCallback(int32_t uid);
  void GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes);
  void SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes);

  int32_t RegisterJoystickPOVsCallback(int32_t joystickNum,
                                       HAL_JoystickPOVsCallback callback,
                                       void* param, HAL_Bool initialNotify);
  void CancelJoystickPOVsCallback(int32_t uid);
  void GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs);
  void SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs);

  int32_t RegisterJoystickButtonsCallback(int32_t joystickNum,
                                          HAL_JoystickButtonsCallback callback,
                                          void* param, HAL_Bool initialNotify);
  void CancelJoystickButtonsCallback(int32_t uid);
  void GetJoystickButtons(int32_t joystickNum, HAL_JoystickButtons* buttons);
  void SetJoystickButtons(int32_t joystickNum,
                          const HAL_JoystickButtons* buttons);

  int32_t RegisterJoystickDescriptorCallback(
      int32_t joystickNum, HAL_JoystickDescriptorCallback callback, void* param,
      HAL_Bool initialNotify);
  void CancelJoystickDescriptorCallback(int32_t uid);
  void GetJoystickDescriptor(int32_t joystickNum,
                             HAL_JoystickDescriptor* descriptor);
  void SetJoystickDescriptor(int32_t joystickNum,
                             const HAL_JoystickDescriptor* descriptor);

  int32_t RegisterJoystickLedsCallback(int32_t joystickNum,
                                       HAL_JoystickLedsCallback callback,
                                       void* param, HAL_Bool initialNotify);
  void CancelJoystickLedsCallback(int32_t uid);
  void GetJoystickLeds(int32_t joystickNum, int32_t* leds);
  void SetJoystickLeds(int32_t joystickNum, int32_t leds);
  int32_t RegisterJoystickRumblesCallback(int32_t joystickNum,
                                          HAL_JoystickRumblesCallback callback,
                                          void* param, HAL_Bool initialNotify);
  void CancelJoystickRumblesCallback(int32_t uid);
  void GetJoystickRumbles(int32_t joystickNum, int32_t* leftRumble,
                          int32_t* rightRumble, int32_t* leftTriggerRumble,
                          int32_t* rightTriggerRumble);
  void SetJoystickRumbles(int32_t joystickNum, int32_t leftRumble,
                          int32_t rightRumble, int32_t leftTriggerRumble,
                          int32_t rightTriggerRumble);

  int32_t RegisterMatchInfoCallback(HAL_MatchInfoCallback callback, void* param,
                                    HAL_Bool initialNotify);
  void CancelMatchInfoCallback(int32_t uid);
  void GetMatchInfo(HAL_MatchInfo* info);
  void SetMatchInfo(const HAL_MatchInfo* info);

  void FreeMatchInfo(const HAL_MatchInfo* info);

  int32_t RegisterNewDataCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelNewDataCallback(int32_t uid);
  void CallNewDataCallbacks();

  void NotifyNewData();

  void SetJoystickButton(int32_t stick, int32_t button, HAL_Bool state);
  void SetJoystickAxis(int32_t stick, int32_t axis, double value);
  void SetJoystickPOV(int32_t stick, int32_t pov, HAL_JoystickPOV value);
  void SetJoystickButtons(int32_t stick, uint64_t buttons);
  void SetJoystickAxesAvailable(int32_t stick, uint16_t available);
  void SetJoystickPOVsAvailable(int32_t stick, uint8_t available);
  void SetJoystickButtonsAvailable(int32_t stick, uint64_t available);
  void GetJoystickAvailables(int32_t stick, uint16_t* axesAvailable,
                             uint64_t* buttonsAvailable,
                             uint8_t* povsAvailable);

  void SetJoystickIsGamepad(int32_t stick, HAL_Bool isGamepad);
  void SetJoystickGamepadType(int32_t stick, int32_t type);
  void SetJoystickName(int32_t stick, std::string_view message);
  void SetJoystickSupportedOutputs(int32_t stick, int32_t supportedOutputs);

  void SetGameSpecificMessage(std::string_view message);
  void SetEventName(std::string_view name);
  void SetMatchType(HAL_MatchType type);
  void SetMatchNumber(int32_t matchNumber);
  void SetReplayNumber(int32_t replayNumber);

  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetEnabledName> enabled{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetAutonomousName> autonomous{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetTestName> test{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetEStopName> eStop{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetFmsAttachedName> fmsAttached{
      false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetDsAttachedName> dsAttached{false};
  SimDataValue<HAL_AllianceStationID, MakeAllianceStationIdValue,
               GetAllianceStationIdName>
      allianceStationId{static_cast<HAL_AllianceStationID>(0)};
  SimDataValue<double, HAL_MakeDouble, GetMatchTimeName> matchTime{-1.0};

 private:
  SimCallbackRegistry<HAL_JoystickAxesCallback, GetJoystickAxesName>
      m_joystickAxesCallbacks;
  SimCallbackRegistry<HAL_JoystickPOVsCallback, GetJoystickPOVsName>
      m_joystickPOVsCallbacks;
  SimCallbackRegistry<HAL_JoystickButtonsCallback, GetJoystickButtonsName>
      m_joystickButtonsCallbacks;
  SimCallbackRegistry<HAL_JoystickLedsCallback, GetJoystickLedsName>
      m_joystickLedsCallbacks;
  SimCallbackRegistry<HAL_JoystickRumblesCallback, GetJoystickRumblesName>
      m_joystickRumblesCallbacks;
  SimCallbackRegistry<HAL_JoystickDescriptorCallback, GetJoystickDescriptorName>
      m_joystickDescriptorCallbacks;
  SimCallbackRegistry<HAL_MatchInfoCallback, GetMatchInfoName>
      m_matchInfoCallbacks;
  SimCallbackRegistry<HAL_NotifyCallback, GetNewDataName> m_newDataCallbacks;

  struct JoystickOutputStore {
    int32_t leds = 0;
    int32_t leftRumble = 0;
    int32_t rightRumble = 0;
    int32_t leftTriggerRumble = 0;
    int32_t rightTriggerRumble = 0;
  };

  struct JoystickData {
    HAL_JoystickAxes axes;
    HAL_JoystickPOVs povs;
    HAL_JoystickButtons buttons;
    JoystickOutputStore outputs;
    HAL_JoystickDescriptor descriptor;
  };

  static constexpr int kNumJoysticks = 6;
  wpi::util::spinlock m_joystickDataMutex;
  JoystickData m_joystickData[kNumJoysticks];

  wpi::util::spinlock m_matchInfoMutex;
  HAL_MatchInfo m_matchInfo;
};
extern DriverStationData* SimDriverStationData;
}  // namespace wpi::hal

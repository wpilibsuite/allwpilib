// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/spinlock.h>
#include <wpi/StringMap.h>

#include "hal/simulation/DriverStationData.h"
#include "hal/simulation/SimCallbackRegistry.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {

class DriverStationData {
  HAL_SIMDATAVALUE_DEFINE_NAME(EStop)
  HAL_SIMDATAVALUE_DEFINE_NAME(FmsAttached)
  HAL_SIMDATAVALUE_DEFINE_NAME(DsAttached)
  HAL_SIMDATAVALUE_DEFINE_NAME(AllianceStationId)
  HAL_SIMDATAVALUE_DEFINE_NAME(MatchTime)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(OpMode)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(SelectedAutonomousOpMode)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(SelectedTeleoperatedOpMode)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(OpModeOptions)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickAxes)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickPOVs)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickButtons)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickDescriptor)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(JoystickOutputs)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(MatchInfo)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(NewData)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE HAL_Value
  MakeAllianceStationIdValue(HAL_AllianceStationID value) {
    return HAL_MakeEnum(value);
  }

 public:
  struct OpModeOption {
    std::string name;
    std::string category;
    std::string description;
    int32_t flags;
  };

  DriverStationData();
  void ResetData();

  int32_t RegisterOpModeCallback(HAL_OpModeCallback callback, void* param,
                                 HAL_Bool initialNotify);
  void CancelOpModeCallback(int32_t uid);
  std::string GetOpMode();
  int32_t GetOpModeId() { return m_opModeId; }
  void SetOpMode(std::string_view opMode);

  int32_t RegisterSelectedAutonomousOpModeCallback(HAL_OpModeCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
  void CancelSelectedAutonomousOpModeCallback(int32_t uid);
  std::string GetSelectedAutonomousOpMode();
  int32_t GetSelectedAutonomousOpModeId() {
    return m_selectedAutonomousOpModeId;
  }
  void SetSelectedAutonomousOpMode(std::string_view opMode);

  int32_t RegisterSelectedTeleoperatedOpModeCallback(
      HAL_OpModeCallback callback, void* param, HAL_Bool initialNotify);
  void CancelSelectedTeleoperatedOpModeCallback(int32_t uid);
  std::string GetSelectedTeleoperatedOpMode();
  int32_t GetSelectedTeleoperatedOpModeId() {
    return m_selectedTeleoperatedOpModeId;
  }
  void SetSelectedTeleoperatedOpMode(std::string_view opMode);

  int32_t RegisterOpModeOptionsCallback(HAL_OpModeOptionsCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelOpModeOptionsCallback(int32_t uid);
  std::vector<OpModeOption> GetOpModeOptions();
  int32_t AddOpModeOption(OpModeOption option);
  int32_t RemoveOpModeOption(std::string_view name);
  void ClearOpModeOptions();

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

  int32_t RegisterJoystickOutputsCallback(int32_t joystickNum,
                                          HAL_JoystickOutputsCallback callback,
                                          void* param, HAL_Bool initialNotify);
  void CancelJoystickOutputsCallback(int32_t uid);
  void GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                          int32_t* leftRumble, int32_t* rightRumble);
  void SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                          int32_t leftRumble, int32_t rightRumble);

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
  void SetJoystickPOV(int32_t stick, int32_t pov, int32_t value);
  void SetJoystickButtons(int32_t stick, uint32_t buttons);
  void SetJoystickAxisCount(int32_t stick, int32_t count);
  void SetJoystickPOVCount(int32_t stick, int32_t count);
  void SetJoystickButtonCount(int32_t stick, int32_t count);
  void GetJoystickCounts(int32_t stick, int32_t* axisCount,
                         int32_t* buttonCount, int32_t* povCount);

  void SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox);
  void SetJoystickType(int32_t stick, int32_t type);
  void SetJoystickName(int32_t stick, std::string_view message);
  void SetJoystickAxisType(int32_t stick, int32_t axis, int32_t type);

  void SetGameSpecificMessage(std::string_view message);
  void SetEventName(std::string_view name);
  void SetMatchType(HAL_MatchType type);
  void SetMatchNumber(int32_t matchNumber);
  void SetReplayNumber(int32_t replayNumber);

  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetEStopName> eStop{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetFmsAttachedName> fmsAttached{
      false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetDsAttachedName> dsAttached{false};
  SimDataValue<HAL_AllianceStationID, MakeAllianceStationIdValue,
               GetAllianceStationIdName>
      allianceStationId{static_cast<HAL_AllianceStationID>(0)};
  SimDataValue<double, HAL_MakeDouble, GetMatchTimeName> matchTime{-1.0};

 private:
  std::vector<HAL_OpModeOption> GetOpModeOptionsInternal();

  SimCallbackRegistry<HAL_OpModeCallback, GetOpModeName> m_opModeCallbacks;
  SimCallbackRegistry<HAL_OpModeCallback, GetSelectedAutonomousOpModeName>
      m_selectedAutonomousOpModeCallbacks;
  SimCallbackRegistry<HAL_OpModeCallback, GetSelectedTeleoperatedOpModeName>
      m_selectedTeleoperatedOpModeCallbacks;
  SimCallbackRegistry<HAL_OpModeOptionsCallback, GetOpModeOptionsName>
      m_opModeOptionsCallbacks;
  SimCallbackRegistry<HAL_JoystickAxesCallback, GetJoystickAxesName>
      m_joystickAxesCallbacks;
  SimCallbackRegistry<HAL_JoystickPOVsCallback, GetJoystickPOVsName>
      m_joystickPOVsCallbacks;
  SimCallbackRegistry<HAL_JoystickButtonsCallback, GetJoystickButtonsName>
      m_joystickButtonsCallbacks;
  SimCallbackRegistry<HAL_JoystickOutputsCallback, GetJoystickOutputsName>
      m_joystickOutputsCallbacks;
  SimCallbackRegistry<HAL_JoystickDescriptorCallback, GetJoystickDescriptorName>
      m_joystickDescriptorCallbacks;
  SimCallbackRegistry<HAL_MatchInfoCallback, GetMatchInfoName>
      m_matchInfoCallbacks;
  SimCallbackRegistry<HAL_NotifyCallback, GetNewDataName> m_newDataCallbacks;

  struct JoystickOutputStore {
    int64_t outputs = 0;
    int32_t leftRumble = 0;
    int32_t rightRumble = 0;
  };

  struct JoystickData {
    HAL_JoystickAxes axes;
    HAL_JoystickPOVs povs;
    HAL_JoystickButtons buttons;
    JoystickOutputStore outputs;
    HAL_JoystickDescriptor descriptor;
  };

  static constexpr int kNumJoysticks = 6;
  wpi::spinlock m_joystickDataMutex;
  JoystickData m_joystickData[kNumJoysticks];

  wpi::spinlock m_matchInfoMutex;
  HAL_MatchInfo m_matchInfo;

  wpi::spinlock m_opModeMutex;
  std::string m_opMode;
  std::string m_selectedAutonomousOpMode;
  std::string m_selectedTeleoperatedOpMode;
  std::atomic<int32_t> m_opModeId = 0;
  std::atomic<int32_t> m_selectedAutonomousOpModeId = 0;
  std::atomic<int32_t> m_selectedTeleoperatedOpModeId = 0;
  std::vector<OpModeOption> m_opModeOptions;
  wpi::StringMap<int32_t> m_opModeMap;
};
extern DriverStationData* SimDriverStationData;
}  // namespace hal

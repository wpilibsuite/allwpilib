/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <atomic>
#include <memory>

#include <support/mutex.h>

#include "MockData/DriverStationData.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {
struct JoystickOutputStore;
struct MatchInfoDataStore;

class DriverStationData {
 public:
  DriverStationData();
  void ResetData();

  int32_t RegisterEnabledCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelEnabledCallback(int32_t uid);
  void InvokeEnabledCallback(HAL_Value value);
  HAL_Bool GetEnabled();
  void SetEnabled(HAL_Bool enabled);

  int32_t RegisterAutonomousCallback(HAL_NotifyCallback callback, void* param,
                                     HAL_Bool initialNotify);
  void CancelAutonomousCallback(int32_t uid);
  void InvokeAutonomousCallback(HAL_Value value);
  HAL_Bool GetAutonomous();
  void SetAutonomous(HAL_Bool autonomous);

  int32_t RegisterTestCallback(HAL_NotifyCallback callback, void* param,
                               HAL_Bool initialNotify);
  void CancelTestCallback(int32_t uid);
  void InvokeTestCallback(HAL_Value value);
  HAL_Bool GetTest();
  void SetTest(HAL_Bool test);

  int32_t RegisterEStopCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelEStopCallback(int32_t uid);
  void InvokeEStopCallback(HAL_Value value);
  HAL_Bool GetEStop();
  void SetEStop(HAL_Bool eStop);

  int32_t RegisterFmsAttachedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelFmsAttachedCallback(int32_t uid);
  void InvokeFmsAttachedCallback(HAL_Value value);
  HAL_Bool GetFmsAttached();
  void SetFmsAttached(HAL_Bool fmsAttached);

  int32_t RegisterDsAttachedCallback(HAL_NotifyCallback callback, void* param,
                                     HAL_Bool initialNotify);
  void CancelDsAttachedCallback(int32_t uid);
  void InvokeDsAttachedCallback(HAL_Value value);
  HAL_Bool GetDsAttached();
  void SetDsAttached(HAL_Bool dsAttached);

  int32_t RegisterAllianceStationIdCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
  void CancelAllianceStationIdCallback(int32_t uid);
  void InvokeAllianceStationIdCallback(HAL_Value value);
  HAL_AllianceStationID GetAllianceStationId();
  void SetAllianceStationId(HAL_AllianceStationID allianceStationId);

  int32_t RegisterMatchTimeCallback(HAL_NotifyCallback callback, void* param,
                                    HAL_Bool initialNotify);
  void CancelMatchTimeCallback(int32_t uid);
  void InvokeMatchTimeCallback(HAL_Value value);
  double GetMatchTime();
  void SetMatchTime(double matchTime);

  void GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes);
  void GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs);
  void GetJoystickButtons(int32_t joystickNum, HAL_JoystickButtons* buttons);
  void GetJoystickDescriptor(int32_t joystickNum,
                             HAL_JoystickDescriptor* descriptor);
  void GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                          int32_t* leftRumble, int32_t* rightRumble);
  void GetMatchInfo(HAL_MatchInfo* info);
  void FreeMatchInfo(const HAL_MatchInfo* info);

  void SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes);
  void SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs);
  void SetJoystickButtons(int32_t joystickNum,
                          const HAL_JoystickButtons* buttons);
  void SetJoystickDescriptor(int32_t joystickNum,
                             const HAL_JoystickDescriptor* descriptor);
  void SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                          int32_t leftRumble, int32_t rightRumble);
  void SetMatchInfo(const HAL_MatchInfo* info);

  void NotifyNewData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_enabled{false};
  std::shared_ptr<NotifyListenerVector> m_enabledCallbacks = nullptr;
  std::atomic<HAL_Bool> m_autonomous{false};
  std::shared_ptr<NotifyListenerVector> m_autonomousCallbacks = nullptr;
  std::atomic<HAL_Bool> m_test{false};
  std::shared_ptr<NotifyListenerVector> m_testCallbacks = nullptr;
  std::atomic<HAL_Bool> m_eStop{false};
  std::shared_ptr<NotifyListenerVector> m_eStopCallbacks = nullptr;
  std::atomic<HAL_Bool> m_fmsAttached{false};
  std::shared_ptr<NotifyListenerVector> m_fmsAttachedCallbacks = nullptr;
  std::atomic<HAL_Bool> m_dsAttached{false};
  std::shared_ptr<NotifyListenerVector> m_dsAttachedCallbacks = nullptr;
  std::atomic<HAL_AllianceStationID> m_allianceStationId{
      static_cast<HAL_AllianceStationID>(0)};
  std::shared_ptr<NotifyListenerVector> m_allianceStationIdCallbacks = nullptr;
  std::atomic<double> m_matchTime{0.0};
  std::shared_ptr<NotifyListenerVector> m_matchTimeCallbacks = nullptr;

  wpi::mutex m_joystickDataMutex;
  wpi::mutex m_matchInfoMutex;

  std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxes;
  std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVs;
  std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtons;

  std::unique_ptr<JoystickOutputStore[]> m_joystickOutputs;
  std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptor;
  std::unique_ptr<MatchInfoDataStore> m_matchInfo;
};
extern DriverStationData* SimDriverStationData;
}  // namespace hal

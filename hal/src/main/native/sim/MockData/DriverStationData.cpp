/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdlib>
#include <cstring>
#include <string>

#include "DriverStationDataInternal.h"
#include "HAL/cpp/make_unique.h"
#include "MockData/NotifyCallbackHelpers.h"

namespace hal {
struct JoystickOutputStore {
  int64_t outputs = 0;
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
};
struct MatchInfoDataStore {
  std::string eventName;
  std::string gameSpecificMessage;
  int32_t replayNumber = 0;
  int32_t matchNumber = 0;
  HAL_MatchType matchType = HAL_MatchType::HAL_kMatchType_none;
};
}  // namespace hal

using namespace hal;

namespace hal {
namespace init {
void InitializeDriverStationData() {
  static DriverStationData dsd;
  ::hal::SimDriverStationData = &dsd;
}
}  // namespace init
}  // namespace hal

DriverStationData* hal::SimDriverStationData;

DriverStationData::DriverStationData() { ResetData(); }

void DriverStationData::ResetData() {
  m_enabled = false;
  m_enabledCallbacks = nullptr;
  m_autonomous = false;
  m_autonomousCallbacks = nullptr;
  m_test = false;
  m_testCallbacks = nullptr;
  m_eStop = false;
  m_eStopCallbacks = nullptr;
  m_fmsAttached = false;
  m_fmsAttachedCallbacks = nullptr;
  m_dsAttached = false;
  m_dsAttachedCallbacks = nullptr;

  {
    std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
    m_joystickAxes = std::make_unique<HAL_JoystickAxes[]>(6);
    m_joystickPOVs = std::make_unique<HAL_JoystickPOVs[]>(6);
    m_joystickButtons = std::make_unique<HAL_JoystickButtons[]>(6);
    m_joystickOutputs = std::make_unique<JoystickOutputStore[]>(6);
    m_joystickDescriptor = std::make_unique<HAL_JoystickDescriptor[]>(6);

    for (int i = 0; i < 6; i++) {
      m_joystickAxes[i].count = 0;
      m_joystickPOVs[i].count = 0;
      m_joystickButtons[i].count = 0;
      m_joystickDescriptor[i].isXbox = 0;
      m_joystickDescriptor[i].type = -1;
      m_joystickDescriptor[i].name[0] = '\0';
    }
  }
  {
    std::lock_guard<wpi::mutex> lock(m_matchInfoMutex);

    m_matchInfo = std::make_unique<MatchInfoDataStore>();
  }
}

int32_t DriverStationData::RegisterEnabledCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_enabledCallbacks = RegisterCallback(m_enabledCallbacks, "Enabled",
                                          callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetEnabled());
    callback("Enabled", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelEnabledCallback(int32_t uid) {
  m_enabledCallbacks = CancelCallback(m_enabledCallbacks, uid);
}

void DriverStationData::InvokeEnabledCallback(HAL_Value value) {
  InvokeCallback(m_enabledCallbacks, "Enabled", &value);
}

HAL_Bool DriverStationData::GetEnabled() { return m_enabled; }

void DriverStationData::SetEnabled(HAL_Bool enabled) {
  HAL_Bool oldValue = m_enabled.exchange(enabled);
  if (oldValue != enabled) {
    InvokeEnabledCallback(MakeBoolean(enabled));
  }
}

int32_t DriverStationData::RegisterAutonomousCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_autonomousCallbacks = RegisterCallback(
        m_autonomousCallbacks, "Autonomous", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetAutonomous());
    callback("Autonomous", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelAutonomousCallback(int32_t uid) {
  m_autonomousCallbacks = CancelCallback(m_autonomousCallbacks, uid);
}

void DriverStationData::InvokeAutonomousCallback(HAL_Value value) {
  InvokeCallback(m_autonomousCallbacks, "Autonomous", &value);
}

HAL_Bool DriverStationData::GetAutonomous() { return m_autonomous; }

void DriverStationData::SetAutonomous(HAL_Bool autonomous) {
  HAL_Bool oldValue = m_autonomous.exchange(autonomous);
  if (oldValue != autonomous) {
    InvokeAutonomousCallback(MakeBoolean(autonomous));
  }
}

int32_t DriverStationData::RegisterTestCallback(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_testCallbacks =
        RegisterCallback(m_testCallbacks, "Test", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetTest());
    callback("Test", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelTestCallback(int32_t uid) {
  m_testCallbacks = CancelCallback(m_testCallbacks, uid);
}

void DriverStationData::InvokeTestCallback(HAL_Value value) {
  InvokeCallback(m_testCallbacks, "Test", &value);
}

HAL_Bool DriverStationData::GetTest() { return m_test; }

void DriverStationData::SetTest(HAL_Bool test) {
  HAL_Bool oldValue = m_test.exchange(test);
  if (oldValue != test) {
    InvokeTestCallback(MakeBoolean(test));
  }
}

int32_t DriverStationData::RegisterEStopCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_eStopCallbacks =
        RegisterCallback(m_eStopCallbacks, "EStop", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetEStop());
    callback("EStop", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelEStopCallback(int32_t uid) {
  m_eStopCallbacks = CancelCallback(m_eStopCallbacks, uid);
}

void DriverStationData::InvokeEStopCallback(HAL_Value value) {
  InvokeCallback(m_eStopCallbacks, "EStop", &value);
}

HAL_Bool DriverStationData::GetEStop() { return m_eStop; }

void DriverStationData::SetEStop(HAL_Bool eStop) {
  HAL_Bool oldValue = m_eStop.exchange(eStop);
  if (oldValue != eStop) {
    InvokeEStopCallback(MakeBoolean(eStop));
  }
}

int32_t DriverStationData::RegisterFmsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_fmsAttachedCallbacks = RegisterCallback(
        m_fmsAttachedCallbacks, "FmsAttached", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetFmsAttached());
    callback("FmsAttached", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelFmsAttachedCallback(int32_t uid) {
  m_fmsAttachedCallbacks = CancelCallback(m_fmsAttachedCallbacks, uid);
}

void DriverStationData::InvokeFmsAttachedCallback(HAL_Value value) {
  InvokeCallback(m_fmsAttachedCallbacks, "FmsAttached", &value);
}

HAL_Bool DriverStationData::GetFmsAttached() { return m_fmsAttached; }

void DriverStationData::SetFmsAttached(HAL_Bool fmsAttached) {
  HAL_Bool oldValue = m_fmsAttached.exchange(fmsAttached);
  if (oldValue != fmsAttached) {
    InvokeFmsAttachedCallback(MakeBoolean(fmsAttached));
  }
}

int32_t DriverStationData::RegisterDsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_dsAttachedCallbacks = RegisterCallback(
        m_dsAttachedCallbacks, "DsAttached", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetDsAttached());
    callback("DsAttached", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelDsAttachedCallback(int32_t uid) {
  m_dsAttachedCallbacks = CancelCallback(m_dsAttachedCallbacks, uid);
}

void DriverStationData::InvokeDsAttachedCallback(HAL_Value value) {
  InvokeCallback(m_dsAttachedCallbacks, "DsAttached", &value);
}

HAL_Bool DriverStationData::GetDsAttached() { return m_dsAttached; }

void DriverStationData::SetDsAttached(HAL_Bool dsAttached) {
  HAL_Bool oldValue = m_dsAttached.exchange(dsAttached);
  if (oldValue != dsAttached) {
    InvokeDsAttachedCallback(MakeBoolean(dsAttached));
  }
}

int32_t DriverStationData::RegisterAllianceStationIdCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_allianceStationIdCallbacks =
        RegisterCallback(m_allianceStationIdCallbacks, "AllianceStationId",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeEnum(GetAllianceStationId());
    callback("AllianceStationId", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelAllianceStationIdCallback(int32_t uid) {
  m_allianceStationIdCallbacks =
      CancelCallback(m_allianceStationIdCallbacks, uid);
}

void DriverStationData::InvokeAllianceStationIdCallback(HAL_Value value) {
  InvokeCallback(m_allianceStationIdCallbacks, "AllianceStationId", &value);
}

HAL_AllianceStationID DriverStationData::GetAllianceStationId() {
  return m_allianceStationId;
}

void DriverStationData::SetAllianceStationId(
    HAL_AllianceStationID allianceStationId) {
  HAL_AllianceStationID oldValue =
      m_allianceStationId.exchange(allianceStationId);
  if (oldValue != allianceStationId) {
    InvokeAllianceStationIdCallback(MakeEnum(allianceStationId));
  }
}

int32_t DriverStationData::RegisterMatchTimeCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_matchTimeCallbacks = RegisterCallback(m_matchTimeCallbacks, "MatchTime",
                                            callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetMatchTime());
    callback("MatchTime", param, &value);
  }
  return newUid;
}

void DriverStationData::CancelMatchTimeCallback(int32_t uid) {
  m_matchTimeCallbacks = CancelCallback(m_matchTimeCallbacks, uid);
}

void DriverStationData::InvokeMatchTimeCallback(HAL_Value value) {
  InvokeCallback(m_matchTimeCallbacks, "MatchTime", &value);
}

double DriverStationData::GetMatchTime() { return m_matchTime; }

void DriverStationData::SetMatchTime(double matchTime) {
  double oldValue = m_matchTime.exchange(matchTime);
  if (oldValue != matchTime) {
    InvokeMatchTimeCallback(MakeDouble(matchTime));
  }
}

void DriverStationData::GetJoystickAxes(int32_t joystickNum,
                                        HAL_JoystickAxes* axes) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  *axes = m_joystickAxes[joystickNum];
}
void DriverStationData::GetJoystickPOVs(int32_t joystickNum,
                                        HAL_JoystickPOVs* povs) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  *povs = m_joystickPOVs[joystickNum];
}
void DriverStationData::GetJoystickButtons(int32_t joystickNum,
                                           HAL_JoystickButtons* buttons) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  *buttons = m_joystickButtons[joystickNum];
}
void DriverStationData::GetJoystickDescriptor(
    int32_t joystickNum, HAL_JoystickDescriptor* descriptor) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  *descriptor = m_joystickDescriptor[joystickNum];
  // Always ensure name is null terminated
  descriptor->name[255] = '\0';
}
void DriverStationData::GetJoystickOutputs(int32_t joystickNum,
                                           int64_t* outputs,
                                           int32_t* leftRumble,
                                           int32_t* rightRumble) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  *leftRumble = m_joystickOutputs[joystickNum].leftRumble;
  *outputs = m_joystickOutputs[joystickNum].outputs;
  *rightRumble = m_joystickOutputs[joystickNum].rightRumble;
}
void DriverStationData::GetMatchInfo(HAL_MatchInfo* info) {
  std::lock_guard<wpi::mutex> lock(m_matchInfoMutex);
  auto eventLen = m_matchInfo->eventName.size();
  info->eventName = static_cast<char*>(std::malloc(eventLen + 1));
  std::memcpy(info->eventName, m_matchInfo->eventName.c_str(), eventLen);
  auto gameLen = m_matchInfo->gameSpecificMessage.size();
  info->gameSpecificMessage = static_cast<char*>(std::malloc(gameLen + 1));
  std::memcpy(info->gameSpecificMessage,
              m_matchInfo->gameSpecificMessage.c_str(), gameLen);
  info->gameSpecificMessage[gameLen] = '\0';
  info->eventName[eventLen] = '\0';
  info->matchNumber = m_matchInfo->matchNumber;
  info->replayNumber = m_matchInfo->replayNumber;
  info->matchType = m_matchInfo->matchType;
}
void DriverStationData::FreeMatchInfo(const HAL_MatchInfo* info) {
  std::free(info->eventName);
  std::free(info->gameSpecificMessage);
}

void DriverStationData::SetJoystickAxes(int32_t joystickNum,
                                        const HAL_JoystickAxes* axes) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  m_joystickAxes[joystickNum] = *axes;
}
void DriverStationData::SetJoystickPOVs(int32_t joystickNum,
                                        const HAL_JoystickPOVs* povs) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  m_joystickPOVs[joystickNum] = *povs;
}
void DriverStationData::SetJoystickButtons(int32_t joystickNum,
                                           const HAL_JoystickButtons* buttons) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  m_joystickButtons[joystickNum] = *buttons;
}

void DriverStationData::SetJoystickDescriptor(
    int32_t joystickNum, const HAL_JoystickDescriptor* descriptor) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  m_joystickDescriptor[joystickNum] = *descriptor;
}

void DriverStationData::SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                                           int32_t leftRumble,
                                           int32_t rightRumble) {
  std::lock_guard<wpi::mutex> lock(m_joystickDataMutex);
  m_joystickOutputs[joystickNum].leftRumble = leftRumble;
  m_joystickOutputs[joystickNum].outputs = outputs;
  m_joystickOutputs[joystickNum].rightRumble = rightRumble;
}

void DriverStationData::SetMatchInfo(const HAL_MatchInfo* info) {
  std::lock_guard<wpi::mutex> lock(m_matchInfoMutex);
  m_matchInfo->eventName = info->eventName;
  m_matchInfo->gameSpecificMessage = info->gameSpecificMessage;
  m_matchInfo->matchNumber = info->matchNumber;
  m_matchInfo->matchType = info->matchType;
  m_matchInfo->replayNumber = info->replayNumber;
}

void DriverStationData::NotifyNewData() { HAL_ReleaseDSMutex(); }

extern "C" {
void HALSIM_ResetDriverStationData(void) { SimDriverStationData->ResetData(); }

int32_t HALSIM_RegisterDriverStationEnabledCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterEnabledCallback(callback, param,
                                                       initialNotify);
}
void HALSIM_CancelDriverStationEnabledCallback(int32_t uid) {
  SimDriverStationData->CancelEnabledCallback(uid);
}
HAL_Bool HALSIM_GetDriverStationEnabled(void) {
  return SimDriverStationData->GetEnabled();
}
void HALSIM_SetDriverStationEnabled(HAL_Bool enabled) {
  SimDriverStationData->SetEnabled(enabled);
}

int32_t HALSIM_RegisterDriverStationAutonomousCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterAutonomousCallback(callback, param,
                                                          initialNotify);
}
void HALSIM_CancelDriverStationAutonomousCallback(int32_t uid) {
  SimDriverStationData->CancelAutonomousCallback(uid);
}
HAL_Bool HALSIM_GetDriverStationAutonomous(void) {
  return SimDriverStationData->GetAutonomous();
}
void HALSIM_SetDriverStationAutonomous(HAL_Bool autonomous) {
  SimDriverStationData->SetAutonomous(autonomous);
}

int32_t HALSIM_RegisterDriverStationTestCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterTestCallback(callback, param,
                                                    initialNotify);
}
void HALSIM_CancelDriverStationTestCallback(int32_t uid) {
  SimDriverStationData->CancelTestCallback(uid);
}
HAL_Bool HALSIM_GetDriverStationTest(void) {
  return SimDriverStationData->GetTest();
}
void HALSIM_SetDriverStationTest(HAL_Bool test) {
  SimDriverStationData->SetTest(test);
}

int32_t HALSIM_RegisterDriverStationEStopCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterEStopCallback(callback, param,
                                                     initialNotify);
}
void HALSIM_CancelDriverStationEStopCallback(int32_t uid) {
  SimDriverStationData->CancelEStopCallback(uid);
}
HAL_Bool HALSIM_GetDriverStationEStop(void) {
  return SimDriverStationData->GetEStop();
}
void HALSIM_SetDriverStationEStop(HAL_Bool eStop) {
  SimDriverStationData->SetEStop(eStop);
}

int32_t HALSIM_RegisterDriverStationFmsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterFmsAttachedCallback(callback, param,
                                                           initialNotify);
}
void HALSIM_CancelDriverStationFmsAttachedCallback(int32_t uid) {
  SimDriverStationData->CancelFmsAttachedCallback(uid);
}
HAL_Bool HALSIM_GetDriverStationFmsAttached(void) {
  return SimDriverStationData->GetFmsAttached();
}
void HALSIM_SetDriverStationFmsAttached(HAL_Bool fmsAttached) {
  SimDriverStationData->SetFmsAttached(fmsAttached);
}

int32_t HALSIM_RegisterDriverStationDsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterDsAttachedCallback(callback, param,
                                                          initialNotify);
}
void HALSIM_CancelDriverStationDsAttachedCallback(int32_t uid) {
  SimDriverStationData->CancelDsAttachedCallback(uid);
}
HAL_Bool HALSIM_GetDriverStationDsAttached(void) {
  return SimDriverStationData->GetDsAttached();
}
void HALSIM_SetDriverStationDsAttached(HAL_Bool dsAttached) {
  SimDriverStationData->SetDsAttached(dsAttached);
}

int32_t HALSIM_RegisterDriverStationAllianceStationIdCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterAllianceStationIdCallback(
      callback, param, initialNotify);
}
void HALSIM_CancelDriverStationAllianceStationIdCallback(int32_t uid) {
  SimDriverStationData->CancelAllianceStationIdCallback(uid);
}
HAL_AllianceStationID HALSIM_GetDriverStationAllianceStationId(void) {
  return SimDriverStationData->GetAllianceStationId();
}
void HALSIM_SetDriverStationAllianceStationId(
    HAL_AllianceStationID allianceStationId) {
  SimDriverStationData->SetAllianceStationId(allianceStationId);
}

int32_t HALSIM_RegisterDriverStationMatchTimeCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterMatchTimeCallback(callback, param,
                                                         initialNotify);
}
void HALSIM_CancelDriverStationMatchTimeCallback(int32_t uid) {
  SimDriverStationData->CancelMatchTimeCallback(uid);
}
double HALSIM_GetDriverStationMatchTime(void) {
  return SimDriverStationData->GetMatchTime();
}
void HALSIM_SetDriverStationMatchTime(double matchTime) {
  SimDriverStationData->SetMatchTime(matchTime);
}

void HALSIM_SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes) {
  SimDriverStationData->SetJoystickAxes(joystickNum, axes);
}

void HALSIM_SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs) {
  SimDriverStationData->SetJoystickPOVs(joystickNum, povs);
}

void HALSIM_SetJoystickButtons(int32_t joystickNum,
                               const HAL_JoystickButtons* buttons) {
  SimDriverStationData->SetJoystickButtons(joystickNum, buttons);
}
void HALSIM_SetJoystickDescriptor(int32_t joystickNum,
                                  const HAL_JoystickDescriptor* descriptor) {
  SimDriverStationData->SetJoystickDescriptor(joystickNum, descriptor);
}

void HALSIM_GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                               int32_t* leftRumble, int32_t* rightRumble) {
  SimDriverStationData->GetJoystickOutputs(joystickNum, outputs, leftRumble,
                                           rightRumble);
}

void HALSIM_SetMatchInfo(const HAL_MatchInfo* info) {
  SimDriverStationData->SetMatchInfo(info);
}

void HALSIM_NotifyDriverStationNewData(void) {
  SimDriverStationData->NotifyNewData();
}

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  SimDriverStationData->RegisterEnabledCallback(callback, param, initialNotify);
  SimDriverStationData->RegisterAutonomousCallback(callback, param,
                                                   initialNotify);
  SimDriverStationData->RegisterTestCallback(callback, param, initialNotify);
  SimDriverStationData->RegisterEStopCallback(callback, param, initialNotify);
  SimDriverStationData->RegisterFmsAttachedCallback(callback, param,
                                                    initialNotify);
  SimDriverStationData->RegisterDsAttachedCallback(callback, param,
                                                   initialNotify);
  SimDriverStationData->RegisterAllianceStationIdCallback(callback, param,
                                                          initialNotify);
  SimDriverStationData->RegisterMatchTimeCallback(callback, param,
                                                  initialNotify);
}
}  // extern "C"

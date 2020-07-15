/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdlib>
#include <cstring>
#include <string>

#include "DriverStationDataInternal.h"
#include "hal/DriverStation.h"

static constexpr int kNumJoysticks = 6;

namespace hal {
struct JoystickOutputStore {
  int64_t outputs = 0;
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
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
  enabled.Reset(false);
  autonomous.Reset(false);
  test.Reset(false);
  eStop.Reset(false);
  fmsAttached.Reset(false);
  dsAttached.Reset(true);
  allianceStationId.Reset(static_cast<HAL_AllianceStationID>(0));
  matchTime.Reset(0.0);

  {
    std::scoped_lock lock(m_joystickDataMutex);
    m_joystickAxes = std::make_unique<HAL_JoystickAxes[]>(kNumJoysticks);
    m_joystickPOVs = std::make_unique<HAL_JoystickPOVs[]>(kNumJoysticks);
    m_joystickButtons = std::make_unique<HAL_JoystickButtons[]>(kNumJoysticks);
    m_joystickOutputs = std::make_unique<JoystickOutputStore[]>(kNumJoysticks);
    m_joystickDescriptor =
        std::make_unique<HAL_JoystickDescriptor[]>(kNumJoysticks);

    for (int i = 0; i < kNumJoysticks; i++) {
      m_joystickAxes[i].count = 0;
      m_joystickPOVs[i].count = 0;
      m_joystickButtons[i].count = 0;
      m_joystickDescriptor[i].isXbox = 0;
      m_joystickDescriptor[i].type = -1;
      m_joystickDescriptor[i].name[0] = '\0';
    }
  }
  {
    std::scoped_lock lock(m_matchInfoMutex);

    m_matchInfo = std::make_unique<HAL_MatchInfo>();
  }
}

void DriverStationData::GetJoystickAxes(int32_t joystickNum,
                                        HAL_JoystickAxes* axes) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  *axes = m_joystickAxes[joystickNum];
}
void DriverStationData::GetJoystickPOVs(int32_t joystickNum,
                                        HAL_JoystickPOVs* povs) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  *povs = m_joystickPOVs[joystickNum];
}
void DriverStationData::GetJoystickButtons(int32_t joystickNum,
                                           HAL_JoystickButtons* buttons) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  *buttons = m_joystickButtons[joystickNum];
}
void DriverStationData::GetJoystickDescriptor(
    int32_t joystickNum, HAL_JoystickDescriptor* descriptor) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  *descriptor = m_joystickDescriptor[joystickNum];
  // Always ensure name is null terminated
  descriptor->name[255] = '\0';
}
void DriverStationData::GetJoystickOutputs(int32_t joystickNum,
                                           int64_t* outputs,
                                           int32_t* leftRumble,
                                           int32_t* rightRumble) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  *leftRumble = m_joystickOutputs[joystickNum].leftRumble;
  *outputs = m_joystickOutputs[joystickNum].outputs;
  *rightRumble = m_joystickOutputs[joystickNum].rightRumble;
}
void DriverStationData::GetMatchInfo(HAL_MatchInfo* info) {
  std::scoped_lock lock(m_matchInfoMutex);
  *info = *m_matchInfo;
}

void DriverStationData::SetJoystickAxes(int32_t joystickNum,
                                        const HAL_JoystickAxes* axes) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickAxes[joystickNum] = *axes;
}
void DriverStationData::SetJoystickPOVs(int32_t joystickNum,
                                        const HAL_JoystickPOVs* povs) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickPOVs[joystickNum] = *povs;
}
void DriverStationData::SetJoystickButtons(int32_t joystickNum,
                                           const HAL_JoystickButtons* buttons) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickButtons[joystickNum] = *buttons;
}

void DriverStationData::SetJoystickDescriptor(
    int32_t joystickNum, const HAL_JoystickDescriptor* descriptor) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickDescriptor[joystickNum] = *descriptor;
}

void DriverStationData::SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                                           int32_t leftRumble,
                                           int32_t rightRumble) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickOutputs[joystickNum].leftRumble = leftRumble;
  m_joystickOutputs[joystickNum].outputs = outputs;
  m_joystickOutputs[joystickNum].rightRumble = rightRumble;
}

void DriverStationData::SetMatchInfo(const HAL_MatchInfo* info) {
  std::scoped_lock lock(m_matchInfoMutex);
  *m_matchInfo = *info;
  *(std::end(m_matchInfo->eventName) - 1) = '\0';
}

void DriverStationData::NotifyNewData() { HAL_ReleaseDSMutex(); }

void DriverStationData::SetJoystickButton(int32_t stick, int32_t button,
                                          HAL_Bool state) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  if (state)
    m_joystickButtons[stick].buttons |= 1 << (button - 1);
  else
    m_joystickButtons[stick].buttons &= ~(1 << (button - 1));
}

void DriverStationData::SetJoystickAxis(int32_t stick, int32_t axis,
                                        double value) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickAxes[stick].axes[axis] = value;
}

void DriverStationData::SetJoystickPOV(int32_t stick, int32_t pov,
                                       int32_t value) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  if (pov < 0 || pov >= HAL_kMaxJoystickPOVs) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickPOVs[stick].povs[pov] = value;
}

void DriverStationData::SetJoystickButtons(int32_t stick, uint32_t buttons) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickButtons[stick].buttons = buttons;
}

void DriverStationData::SetJoystickAxisCount(int32_t stick, int32_t count) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickAxes[stick].count = count;
  m_joystickDescriptor[stick].axisCount = count;
}

void DriverStationData::SetJoystickPOVCount(int32_t stick, int32_t count) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickPOVs[stick].count = count;
  m_joystickDescriptor[stick].povCount = count;
}

void DriverStationData::SetJoystickButtonCount(int32_t stick, int32_t count) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickButtons[stick].count = count;
  m_joystickDescriptor[stick].buttonCount = count;
}

void DriverStationData::SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickDescriptor[stick].isXbox = isXbox;
}

void DriverStationData::SetJoystickType(int32_t stick, int32_t type) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickDescriptor[stick].type = type;
}

void DriverStationData::SetJoystickName(int32_t stick, const char* name) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  std::scoped_lock lock(m_joystickDataMutex);
  std::strncpy(m_joystickDescriptor[stick].name, name,
               sizeof(m_joystickDescriptor[stick].name) - 1);
}

void DriverStationData::SetJoystickAxisType(int32_t stick, int32_t axis,
                                            int32_t type) {
  if (stick < 0 || stick >= kNumJoysticks) return;
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) return;
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickDescriptor[stick].axisTypes[axis] = type;
}

void DriverStationData::SetGameSpecificMessage(const char* message) {
  std::scoped_lock lock(m_matchInfoMutex);
  std::strncpy(reinterpret_cast<char*>(m_matchInfo->gameSpecificMessage),
               message, sizeof(m_matchInfo->gameSpecificMessage) - 1);
  *(std::end(m_matchInfo->gameSpecificMessage) - 1) = '\0';
  m_matchInfo->gameSpecificMessageSize = std::strlen(message);
}

void DriverStationData::SetEventName(const char* name) {
  std::scoped_lock lock(m_matchInfoMutex);
  std::strncpy(m_matchInfo->eventName, name,
               sizeof(m_matchInfo->eventName) - 1);
  *(std::end(m_matchInfo->eventName) - 1) = '\0';
}

void DriverStationData::SetMatchType(HAL_MatchType type) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo->matchType = type;
}

void DriverStationData::SetMatchNumber(int32_t matchNumber) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo->matchNumber = matchNumber;
}

void DriverStationData::SetReplayNumber(int32_t replayNumber) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo->replayNumber = replayNumber;
}

extern "C" {
void HALSIM_ResetDriverStationData(void) { SimDriverStationData->ResetData(); }

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                                \
  HAL_SIMDATAVALUE_DEFINE_CAPI_NOINDEX(TYPE, HALSIM, DriverStation##CAPINAME, \
                                       SimDriverStationData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Enabled, enabled)
DEFINE_CAPI(HAL_Bool, Autonomous, autonomous)
DEFINE_CAPI(HAL_Bool, Test, test)
DEFINE_CAPI(HAL_Bool, EStop, eStop)
DEFINE_CAPI(HAL_Bool, FmsAttached, fmsAttached)
DEFINE_CAPI(HAL_Bool, DsAttached, dsAttached)
DEFINE_CAPI(HAL_AllianceStationID, AllianceStationId, allianceStationId)
DEFINE_CAPI(double, MatchTime, matchTime)

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

void HALSIM_SetJoystickButton(int32_t stick, int32_t button, HAL_Bool state) {
  SimDriverStationData->SetJoystickButton(stick, button, state);
}

void HALSIM_SetJoystickAxis(int32_t stick, int32_t axis, double value) {
  SimDriverStationData->SetJoystickAxis(stick, axis, value);
}

void HALSIM_SetJoystickPOV(int32_t stick, int32_t pov, int32_t value) {
  SimDriverStationData->SetJoystickPOV(stick, pov, value);
}

void HALSIM_SetJoystickButtonsValue(int32_t stick, uint32_t buttons) {
  SimDriverStationData->SetJoystickButtons(stick, buttons);
}

void HALSIM_SetJoystickAxisCount(int32_t stick, int32_t count) {
  SimDriverStationData->SetJoystickAxisCount(stick, count);
}

void HALSIM_SetJoystickPOVCount(int32_t stick, int32_t count) {
  SimDriverStationData->SetJoystickPOVCount(stick, count);
}

void HALSIM_SetJoystickButtonCount(int32_t stick, int32_t count) {
  SimDriverStationData->SetJoystickButtonCount(stick, count);
}

void HALSIM_SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox) {
  SimDriverStationData->SetJoystickIsXbox(stick, isXbox);
}

void HALSIM_SetJoystickType(int32_t stick, int32_t type) {
  SimDriverStationData->SetJoystickType(stick, type);
}

void HALSIM_SetJoystickName(int32_t stick, const char* name) {
  SimDriverStationData->SetJoystickName(stick, name);
}

void HALSIM_SetJoystickAxisType(int32_t stick, int32_t axis, int32_t type) {
  SimDriverStationData->SetJoystickAxisType(stick, axis, type);
}

void HALSIM_SetGameSpecificMessage(const char* message) {
  SimDriverStationData->SetGameSpecificMessage(message);
}

void HALSIM_SetEventName(const char* name) {
  SimDriverStationData->SetEventName(name);
}

void HALSIM_SetMatchType(HAL_MatchType type) {
  SimDriverStationData->SetMatchType(type);
}

void HALSIM_SetMatchNumber(int32_t matchNumber) {
  SimDriverStationData->SetMatchNumber(matchNumber);
}

void HALSIM_SetReplayNumber(int32_t replayNumber) {
  SimDriverStationData->SetReplayNumber(replayNumber);
}

#define REGISTER(NAME) \
  SimDriverStationData->NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  REGISTER(enabled);
  REGISTER(autonomous);
  REGISTER(test);
  REGISTER(eStop);
  REGISTER(fmsAttached);
  REGISTER(dsAttached);
  REGISTER(allianceStationId);
  REGISTER(matchTime);
}
}  // extern "C"

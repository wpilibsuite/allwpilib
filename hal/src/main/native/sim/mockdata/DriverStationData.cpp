// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstring>

#include "DriverStationDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeDriverStationData() {
  static DriverStationData dsd;
  ::hal::SimDriverStationData = &dsd;
}
}  // namespace hal::init

DriverStationData* hal::SimDriverStationData;

DriverStationData::DriverStationData() {
  ResetData();
}

void DriverStationData::ResetData() {
  enabled.Reset(false);
  autonomous.Reset(false);
  test.Reset(false);
  eStop.Reset(false);
  fmsAttached.Reset(false);
  dsAttached.Reset(false);
  allianceStationId.Reset(static_cast<HAL_AllianceStationID>(0));
  matchTime.Reset(-1.0);

  {
    std::scoped_lock lock(m_joystickDataMutex);
    m_joystickAxesCallbacks.Reset();
    m_joystickPOVsCallbacks.Reset();
    m_joystickButtonsCallbacks.Reset();
    m_joystickOutputsCallbacks.Reset();
    m_joystickDescriptorCallbacks.Reset();
    for (int i = 0; i < kNumJoysticks; i++) {
      m_joystickData[i].axes = HAL_JoystickAxes{};
      m_joystickData[i].povs = HAL_JoystickPOVs{};
      m_joystickData[i].buttons = HAL_JoystickButtons{};
      m_joystickData[i].descriptor = HAL_JoystickDescriptor{};
      m_joystickData[i].descriptor.type = -1;
      m_joystickData[i].descriptor.name[0] = '\0';
    }
  }
  {
    std::scoped_lock lock(m_matchInfoMutex);
    m_matchInfoCallbacks.Reset();
    m_matchInfo = HAL_MatchInfo{};
  }
  m_newDataCallbacks.Reset();
}

#define DEFINE_CPPAPI_CALLBACKS(name, data, data2)                             \
  int32_t DriverStationData::RegisterJoystick##name##Callback(                 \
      int32_t joystickNum, HAL_Joystick##name##Callback callback, void* param, \
      HAL_Bool initialNotify) {                                                \
    if (joystickNum < 0 || joystickNum >= kNumJoysticks)                       \
      return 0;                                                                \
    std::scoped_lock lock(m_joystickDataMutex);                                \
    int32_t uid = m_joystick##name##Callbacks.Register(callback, param);       \
    if (initialNotify) {                                                       \
      callback(GetJoystick##name##Name(), param, joystickNum,                  \
               &m_joystickData[joystickNum].data##data2);                      \
    }                                                                          \
    return uid;                                                                \
  }                                                                            \
                                                                               \
  void DriverStationData::CancelJoystick##name##Callback(int32_t uid) {        \
    m_joystick##name##Callbacks.Cancel(uid);                                   \
  }

#define DEFINE_CPPAPI(name, data, data2)                                   \
  DEFINE_CPPAPI_CALLBACKS(name, data, data2)                               \
                                                                           \
  void DriverStationData::GetJoystick##name(int32_t joystickNum,           \
                                            HAL_Joystick##name* d) {       \
    if (joystickNum < 0 || joystickNum >= kNumJoysticks)                   \
      return;                                                              \
    std::scoped_lock lock(m_joystickDataMutex);                            \
    *d = m_joystickData[joystickNum].data##data2;                          \
  }                                                                        \
                                                                           \
  void DriverStationData::SetJoystick##name(int32_t joystickNum,           \
                                            const HAL_Joystick##name* d) { \
    if (joystickNum < 0 || joystickNum >= kNumJoysticks)                   \
      return;                                                              \
    std::scoped_lock lock(m_joystickDataMutex);                            \
    m_joystickData[joystickNum].data##data2 = *d;                          \
    m_joystick##name##Callbacks(joystickNum, d);                           \
  }

DEFINE_CPPAPI(Axes, axes, )
DEFINE_CPPAPI(POVs, povs, )
DEFINE_CPPAPI(Buttons, buttons, )

DEFINE_CPPAPI_CALLBACKS(Descriptor, descriptor, )

void DriverStationData::GetJoystickDescriptor(
    int32_t joystickNum, HAL_JoystickDescriptor* descriptor) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  *descriptor = m_joystickData[joystickNum].descriptor;
}

void DriverStationData::SetJoystickDescriptor(
    int32_t joystickNum, const HAL_JoystickDescriptor* descriptor) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[joystickNum].descriptor = *descriptor;
  // Always ensure name is null terminated
  m_joystickData[joystickNum].descriptor.name[255] = '\0';
  m_joystickDescriptorCallbacks(joystickNum, descriptor);
}

int32_t DriverStationData::RegisterJoystickOutputsCallback(
    int32_t joystickNum, HAL_JoystickOutputsCallback callback, void* param,
    HAL_Bool initialNotify) {
  if (joystickNum < 0 || joystickNum >= DriverStationData::kNumJoysticks) {
    return 0;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  int32_t uid = m_joystickOutputsCallbacks.Register(callback, param);
  if (initialNotify) {
    const auto& outputs = m_joystickData[joystickNum].outputs;
    callback(DriverStationData::GetJoystickOutputsName(), param, joystickNum,
             outputs.outputs, outputs.leftRumble, outputs.rightRumble);
  }
  return uid;
}

void DriverStationData::CancelJoystickOutputsCallback(int32_t uid) {
  m_joystickOutputsCallbacks.Cancel(uid);
}

void DriverStationData::GetJoystickOutputs(int32_t joystickNum,
                                           int64_t* outputs,
                                           int32_t* leftRumble,
                                           int32_t* rightRumble) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  *leftRumble = m_joystickData[joystickNum].outputs.leftRumble;
  *outputs = m_joystickData[joystickNum].outputs.outputs;
  *rightRumble = m_joystickData[joystickNum].outputs.rightRumble;
}

void DriverStationData::SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                                           int32_t leftRumble,
                                           int32_t rightRumble) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[joystickNum].outputs.leftRumble = leftRumble;
  m_joystickData[joystickNum].outputs.outputs = outputs;
  m_joystickData[joystickNum].outputs.rightRumble = rightRumble;
  m_joystickOutputsCallbacks(joystickNum, outputs, leftRumble, rightRumble);
}

int32_t DriverStationData::RegisterMatchInfoCallback(
    HAL_MatchInfoCallback callback, void* param, HAL_Bool initialNotify) {
  std::scoped_lock lock(m_matchInfoMutex);
  int32_t uid = m_matchInfoCallbacks.Register(callback, param);
  if (initialNotify) {
    callback(GetMatchInfoName(), param, &m_matchInfo);
  }
  return uid;
}

void DriverStationData::CancelMatchInfoCallback(int32_t uid) {
  m_matchInfoCallbacks.Cancel(uid);
}

void DriverStationData::GetMatchInfo(HAL_MatchInfo* info) {
  std::scoped_lock lock(m_matchInfoMutex);
  *info = m_matchInfo;
}

void DriverStationData::SetMatchInfo(const HAL_MatchInfo* info) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo = *info;
  *(std::end(m_matchInfo.eventName) - 1) = '\0';
  m_matchInfoCallbacks(info);
}

int32_t DriverStationData::RegisterNewDataCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  int32_t uid = m_newDataCallbacks.Register(callback, param);
  if (initialNotify) {
    HAL_Value empty;
    empty.type = HAL_UNASSIGNED;
    callback(GetNewDataName(), param, &empty);
  }
  return uid;
}

void DriverStationData::CancelNewDataCallback(int32_t uid) {
  m_newDataCallbacks.Cancel(uid);
}

void DriverStationData::CallNewDataCallbacks() {
  HAL_Value empty;
  empty.type = HAL_UNASSIGNED;
  m_newDataCallbacks(&empty);
}

namespace hal {
void NewDriverStationData();
}  // namespace hal

void DriverStationData::NotifyNewData() {
  hal::NewDriverStationData();
}

void DriverStationData::SetJoystickButton(int32_t stick, int32_t button,
                                          HAL_Bool state) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  if (state) {
    m_joystickData[stick].buttons.buttons |= 1 << (button - 1);
  } else {
    m_joystickData[stick].buttons.buttons &= ~(1 << (button - 1));
  }
  m_joystickButtonsCallbacks(stick, &m_joystickData[stick].buttons);
}

void DriverStationData::SetJoystickAxis(int32_t stick, int32_t axis,
                                        double value) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].axes.axes[axis] = value;
  m_joystickAxesCallbacks(stick, &m_joystickData[stick].axes);
}

void DriverStationData::SetJoystickPOV(int32_t stick, int32_t pov,
                                       int32_t value) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  if (pov < 0 || pov >= HAL_kMaxJoystickPOVs) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].povs.povs[pov] = value;
  m_joystickPOVsCallbacks(stick, &m_joystickData[stick].povs);
}

void DriverStationData::SetJoystickButtons(int32_t stick, uint32_t buttons) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].buttons.buttons = buttons;
  m_joystickButtonsCallbacks(stick, &m_joystickData[stick].buttons);
}

void DriverStationData::SetJoystickAxisCount(int32_t stick, int32_t count) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].axes.count = count;
  m_joystickData[stick].descriptor.axisCount = count;
  m_joystickAxesCallbacks(stick, &m_joystickData[stick].axes);
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickPOVCount(int32_t stick, int32_t count) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].povs.count = count;
  m_joystickData[stick].descriptor.povCount = count;
  m_joystickPOVsCallbacks(stick, &m_joystickData[stick].povs);
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickButtonCount(int32_t stick, int32_t count) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].buttons.count = count;
  m_joystickData[stick].descriptor.buttonCount = count;
  m_joystickButtonsCallbacks(stick, &m_joystickData[stick].buttons);
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::GetJoystickCounts(int32_t stick, int32_t* axisCount,
                                          int32_t* buttonCount,
                                          int32_t* povCount) {
  if (stick < 0 || stick >= kNumJoysticks) {
    *axisCount = 0;
    *buttonCount = 0;
    *povCount = 0;
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  *axisCount = m_joystickData[stick].axes.count;
  *buttonCount = m_joystickData[stick].buttons.count;
  *povCount = m_joystickData[stick].povs.count;
}

void DriverStationData::SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].descriptor.isXbox = isXbox;
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickType(int32_t stick, int32_t type) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].descriptor.type = type;
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickName(int32_t stick, std::string_view name) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  auto copied = name.copy(m_joystickData[stick].descriptor.name,
                          sizeof(m_joystickData[stick].descriptor.name) - 1);
  m_joystickData[stick].descriptor.name[copied] = '\0';
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickAxisType(int32_t stick, int32_t axis,
                                            int32_t type) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].descriptor.axisTypes[axis] = type;
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetGameSpecificMessage(std::string_view message) {
  std::scoped_lock lock(m_matchInfoMutex);
  auto copied =
      message.copy(reinterpret_cast<char*>(m_matchInfo.gameSpecificMessage),
                   sizeof(m_matchInfo.gameSpecificMessage));
  m_matchInfo.gameSpecificMessageSize = copied;
  m_matchInfoCallbacks(&m_matchInfo);
}

void DriverStationData::SetEventName(std::string_view name) {
  std::scoped_lock lock(m_matchInfoMutex);
  auto copied =
      name.copy(m_matchInfo.eventName, sizeof(m_matchInfo.eventName) - 1);
  m_matchInfo.eventName[copied] = '\0';
  m_matchInfoCallbacks(&m_matchInfo);
}

void DriverStationData::SetMatchType(HAL_MatchType type) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo.matchType = type;
  m_matchInfoCallbacks(&m_matchInfo);
}

void DriverStationData::SetMatchNumber(int32_t matchNumber) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo.matchNumber = matchNumber;
  m_matchInfoCallbacks(&m_matchInfo);
}

void DriverStationData::SetReplayNumber(int32_t replayNumber) {
  std::scoped_lock lock(m_matchInfoMutex);
  m_matchInfo.replayNumber = replayNumber;
  m_matchInfoCallbacks(&m_matchInfo);
}

extern "C" {
void HALSIM_ResetDriverStationData(void) {
  SimDriverStationData->ResetData();
}

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

#undef DEFINE_CAPI
#define DEFINE_CAPI(name, data)                                                \
  int32_t HALSIM_RegisterJoystick##name##Callback(                             \
      int32_t joystickNum, HAL_Joystick##name##Callback callback, void* param, \
      HAL_Bool initialNotify) {                                                \
    return SimDriverStationData->RegisterJoystick##name##Callback(             \
        joystickNum, callback, param, initialNotify);                          \
  }                                                                            \
                                                                               \
  void HALSIM_CancelJoystick##name##Callback(int32_t uid) {                    \
    SimDriverStationData->CancelJoystick##name##Callback(uid);                 \
  }                                                                            \
                                                                               \
  void HALSIM_GetJoystick##name(int32_t joystickNum, HAL_Joystick##name* d) {  \
    SimDriverStationData->GetJoystick##name(joystickNum, d);                   \
  }                                                                            \
                                                                               \
  void HALSIM_SetJoystick##name(int32_t joystickNum,                           \
                                const HAL_Joystick##name* d) {                 \
    SimDriverStationData->SetJoystick##name(joystickNum, d);                   \
  }

DEFINE_CAPI(Axes, axes)
DEFINE_CAPI(POVs, povs)
DEFINE_CAPI(Buttons, buttons)
DEFINE_CAPI(Descriptor, descriptor)

int32_t HALSIM_RegisterJoystickOutputsCallback(
    int32_t joystickNum, HAL_JoystickOutputsCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterJoystickOutputsCallback(
      joystickNum, callback, param, initialNotify);
}

void HALSIM_CancelJoystickOutputsCallback(int32_t uid) {
  SimDriverStationData->CancelJoystickOutputsCallback(uid);
}

void HALSIM_GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                               int32_t* leftRumble, int32_t* rightRumble) {
  SimDriverStationData->GetJoystickOutputs(joystickNum, outputs, leftRumble,
                                           rightRumble);
}

void HALSIM_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble) {
  SimDriverStationData->SetJoystickOutputs(joystickNum, outputs, leftRumble,
                                           rightRumble);
}

int32_t HALSIM_RegisterMatchInfoCallback(HAL_MatchInfoCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterMatchInfoCallback(callback, param,
                                                         initialNotify);
}

void HALSIM_CancelMatchInfoCallback(int32_t uid) {
  SimDriverStationData->CancelMatchInfoCallback(uid);
}

void HALSIM_GetMatchInfo(HAL_MatchInfo* info) {
  SimDriverStationData->GetMatchInfo(info);
}

void HALSIM_SetMatchInfo(const HAL_MatchInfo* info) {
  SimDriverStationData->SetMatchInfo(info);
}

int32_t HALSIM_RegisterDriverStationNewDataCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterNewDataCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelDriverStationNewDataCallback(int32_t uid) {
  SimDriverStationData->CancelNewDataCallback(uid);
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

void HALSIM_GetJoystickCounts(int32_t stick, int32_t* axisCount,
                              int32_t* buttonCount, int32_t* povCount) {
  SimDriverStationData->GetJoystickCounts(stick, axisCount, buttonCount,
                                          povCount);
}

void HALSIM_SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox) {
  SimDriverStationData->SetJoystickIsXbox(stick, isXbox);
}

void HALSIM_SetJoystickType(int32_t stick, int32_t type) {
  SimDriverStationData->SetJoystickType(stick, type);
}

void HALSIM_SetJoystickName(int32_t stick, const WPI_String* name) {
  SimDriverStationData->SetJoystickName(stick, wpi::to_string_view(name));
}

void HALSIM_SetJoystickAxisType(int32_t stick, int32_t axis, int32_t type) {
  SimDriverStationData->SetJoystickAxisType(stick, axis, type);
}

void HALSIM_SetGameSpecificMessage(const WPI_String* message) {
  SimDriverStationData->SetGameSpecificMessage(wpi::to_string_view(message));
}

void HALSIM_SetEventName(const WPI_String* name) {
  SimDriverStationData->SetEventName(wpi::to_string_view(name));
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

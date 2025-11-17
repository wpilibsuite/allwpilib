// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstring>

#include "DriverStationDataInternal.h"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeDriverStationData() {
  static DriverStationData dsd;
  ::wpi::hal::SimDriverStationData = &dsd;
}
}  // namespace wpi::hal::init

DriverStationData* wpi::hal::SimDriverStationData;

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
    m_joystickLedsCallbacks.Reset();
    m_joystickRumblesCallbacks.Reset();
    m_joystickDescriptorCallbacks.Reset();
    for (int i = 0; i < kNumJoysticks; i++) {
      m_joystickData[i].axes = HAL_JoystickAxes{};
      m_joystickData[i].povs = HAL_JoystickPOVs{};
      m_joystickData[i].buttons = HAL_JoystickButtons{};
      m_joystickData[i].descriptor = HAL_JoystickDescriptor{};
      m_joystickData[i].outputs.leds = 0;
      m_joystickData[i].outputs.leftRumble = 0;
      m_joystickData[i].outputs.rightRumble = 0;
      m_joystickData[i].outputs.leftTriggerRumble = 0;
      m_joystickData[i].outputs.rightTriggerRumble = 0;
      m_joystickData[i].descriptor.gamepadType = 0;
      m_joystickData[i].descriptor.isGamepad = 0;
      m_joystickData[i].descriptor.supportedOutputs = 0;
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

int32_t DriverStationData::RegisterJoystickLedsCallback(
    int32_t joystickNum, HAL_JoystickLedsCallback callback, void* param,
    HAL_Bool initialNotify) {
  if (joystickNum < 0 || joystickNum >= DriverStationData::kNumJoysticks) {
    return 0;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  int32_t uid = m_joystickLedsCallbacks.Register(callback, param);
  if (initialNotify) {
    const auto& outputs = m_joystickData[joystickNum].outputs;
    callback(DriverStationData::GetJoystickLedsName(), param, joystickNum,
             outputs.leds);
  }
  return uid;
}

void DriverStationData::CancelJoystickLedsCallback(int32_t uid) {
  m_joystickLedsCallbacks.Cancel(uid);
}

void DriverStationData::GetJoystickLeds(int32_t joystickNum, int32_t* leds) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  *leds = m_joystickData[joystickNum].outputs.leds;
}

void DriverStationData::SetJoystickLeds(int32_t joystickNum, int32_t leds) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[joystickNum].outputs.leds = leds;
  m_joystickLedsCallbacks(joystickNum, leds);
}

int32_t DriverStationData::RegisterJoystickRumblesCallback(
    int32_t joystickNum, HAL_JoystickRumblesCallback callback, void* param,
    HAL_Bool initialNotify) {
  if (joystickNum < 0 || joystickNum >= DriverStationData::kNumJoysticks) {
    return 0;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  int32_t uid = m_joystickRumblesCallbacks.Register(callback, param);
  if (initialNotify) {
    const auto& outputs = m_joystickData[joystickNum].outputs;
    callback(DriverStationData::GetJoystickRumblesName(), param, joystickNum,
             outputs.leftRumble, outputs.rightRumble, outputs.leftTriggerRumble,
             outputs.rightTriggerRumble);
  }
  return uid;
}

void DriverStationData::CancelJoystickRumblesCallback(int32_t uid) {
  m_joystickRumblesCallbacks.Cancel(uid);
}

void DriverStationData::GetJoystickRumbles(int32_t joystickNum,
                                           int32_t* leftRumble,
                                           int32_t* rightRumble,
                                           int32_t* leftTriggerRumble,
                                           int32_t* rightTriggerRumble) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  const auto& outputs = m_joystickData[joystickNum].outputs;
  *leftRumble = outputs.leftRumble;
  *rightRumble = outputs.rightRumble;
  *leftTriggerRumble = outputs.leftTriggerRumble;
  *rightTriggerRumble = outputs.rightTriggerRumble;
}

void DriverStationData::SetJoystickRumbles(int32_t joystickNum,
                                           int32_t leftRumble,
                                           int32_t rightRumble,
                                           int32_t leftTriggerRumble,
                                           int32_t rightTriggerRumble) {
  if (joystickNum < 0 || joystickNum >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  auto& outputs = m_joystickData[joystickNum].outputs;
  outputs.leftRumble = leftRumble;
  outputs.rightRumble = rightRumble;
  outputs.leftTriggerRumble = leftTriggerRumble;
  outputs.rightTriggerRumble = rightTriggerRumble;
  m_joystickRumblesCallbacks(joystickNum, leftRumble, rightRumble,
                             leftTriggerRumble, rightTriggerRumble);
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

namespace wpi::hal {
void NewDriverStationData();
}  // namespace wpi::hal

void DriverStationData::NotifyNewData() {
  wpi::hal::NewDriverStationData();
}

void DriverStationData::SetJoystickButton(int32_t stick, int32_t button,
                                          HAL_Bool state) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  if (state) {
    m_joystickData[stick].buttons.buttons |= 1llu << button;
  } else {
    m_joystickData[stick].buttons.buttons &= ~(1llu << button);
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
                                       HAL_JoystickPOV value) {
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

void DriverStationData::SetJoystickButtons(int32_t stick, uint64_t buttons) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].buttons.buttons = buttons;
  m_joystickButtonsCallbacks(stick, &m_joystickData[stick].buttons);
}

void DriverStationData::SetJoystickAxesAvailable(int32_t stick,
                                                 uint16_t available) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].axes.available = available;
  m_joystickAxesCallbacks(stick, &m_joystickData[stick].axes);
}

void DriverStationData::SetJoystickPOVsAvailable(int32_t stick,
                                                 uint8_t available) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].povs.available = available;
  m_joystickPOVsCallbacks(stick, &m_joystickData[stick].povs);
}

void DriverStationData::SetJoystickButtonsAvailable(int32_t stick,
                                                    uint64_t available) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].buttons.available = available;
  m_joystickButtonsCallbacks(stick, &m_joystickData[stick].buttons);
}

void DriverStationData::GetJoystickAvailables(int32_t stick,
                                              uint16_t* axesAvailable,
                                              uint64_t* buttonsAvailable,
                                              uint8_t* povsAvailable) {
  if (stick < 0 || stick >= kNumJoysticks) {
    *axesAvailable = 0;
    *buttonsAvailable = 0;
    *povsAvailable = 0;
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  *axesAvailable = m_joystickData[stick].axes.available;
  *buttonsAvailable = m_joystickData[stick].buttons.available;
  *povsAvailable = m_joystickData[stick].povs.available;
}

void DriverStationData::SetJoystickIsGamepad(int32_t stick,
                                             HAL_Bool isGamepad) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].descriptor.isGamepad = isGamepad;
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickGamepadType(int32_t stick, int32_t type) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].descriptor.gamepadType = type;
  m_joystickDescriptorCallbacks(stick, &m_joystickData[stick].descriptor);
}

void DriverStationData::SetJoystickSupportedOutputs(int32_t stick,
                                                    int32_t supportedOutputs) {
  if (stick < 0 || stick >= kNumJoysticks) {
    return;
  }
  std::scoped_lock lock(m_joystickDataMutex);
  m_joystickData[stick].descriptor.supportedOutputs = supportedOutputs;
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

int32_t HALSIM_RegisterJoystickLedsCallback(int32_t joystickNum,
                                            HAL_JoystickLedsCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterJoystickLedsCallback(
      joystickNum, callback, param, initialNotify);
}

void HALSIM_CancelJoystickLedsCallback(int32_t uid) {
  SimDriverStationData->CancelJoystickLedsCallback(uid);
}

void HALSIM_GetJoystickLeds(int32_t joystickNum, int32_t* leds) {
  SimDriverStationData->GetJoystickLeds(joystickNum, leds);
}

void HALSIM_SetJoystickLeds(int32_t joystickNum, int32_t leds) {
  SimDriverStationData->SetJoystickLeds(joystickNum, leds);
}

int32_t HALSIM_RegisterJoystickRumblesCallback(
    int32_t joystickNum, HAL_JoystickRumblesCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimDriverStationData->RegisterJoystickRumblesCallback(
      joystickNum, callback, param, initialNotify);
}

void HALSIM_CancelJoystickRumblesCallback(int32_t uid) {
  SimDriverStationData->CancelJoystickRumblesCallback(uid);
}

void HALSIM_GetJoystickRumbles(int32_t joystickNum, int32_t* leftRumble,
                               int32_t* rightRumble, int32_t* leftTriggerRumble,
                               int32_t* rightTriggerRumble) {
  SimDriverStationData->GetJoystickRumbles(joystickNum, leftRumble, rightRumble,
                                           leftTriggerRumble,
                                           rightTriggerRumble);
}

void HALSIM_SetJoystickRumbles(int32_t joystickNum, int32_t leftRumble,
                               int32_t rightRumble, int32_t leftTriggerRumble,
                               int32_t rightTriggerRumble) {
  SimDriverStationData->SetJoystickRumbles(joystickNum, leftRumble, rightRumble,
                                           leftTriggerRumble,
                                           rightTriggerRumble);
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

void HALSIM_SetJoystickPOV(int32_t stick, int32_t pov, HAL_JoystickPOV value) {
  SimDriverStationData->SetJoystickPOV(stick, pov, value);
}

void HALSIM_SetJoystickButtonsValue(int32_t stick, uint64_t buttons) {
  SimDriverStationData->SetJoystickButtons(stick, buttons);
}

void HALSIM_SetJoystickAxesAvailable(int32_t stick, uint16_t available) {
  SimDriverStationData->SetJoystickAxesAvailable(stick, available);
}

void HALSIM_SetJoystickPOVsAvailable(int32_t stick, uint8_t available) {
  SimDriverStationData->SetJoystickPOVsAvailable(stick, available);
}

void HALSIM_SetJoystickButtonsAvailable(int32_t stick, uint64_t available) {
  SimDriverStationData->SetJoystickButtonsAvailable(stick, available);
}

void HALSIM_GetJoystickAvailables(int32_t stick, uint16_t* axesAvailable,
                                  uint64_t* buttonsAvailable,
                                  uint8_t* povsAvailable) {
  SimDriverStationData->GetJoystickAvailables(stick, axesAvailable,
                                              buttonsAvailable, povsAvailable);
}

void HALSIM_SetJoystickIsGamepad(int32_t stick, HAL_Bool isGamepad) {
  SimDriverStationData->SetJoystickIsGamepad(stick, isGamepad);
}

void HALSIM_SetJoystickGamepadType(int32_t stick, int32_t type) {
  SimDriverStationData->SetJoystickGamepadType(stick, type);
}

void HALSIM_SetJoystickSupportedOutputs(int32_t stick,
                                        int32_t supportedOutputs) {
  SimDriverStationData->SetJoystickSupportedOutputs(stick, supportedOutputs);
}

void HALSIM_SetJoystickName(int32_t stick, const WPI_String* name) {
  SimDriverStationData->SetJoystickName(stick, wpi::util::to_string_view(name));
}

void HALSIM_SetGameSpecificMessage(const WPI_String* message) {
  SimDriverStationData->SetGameSpecificMessage(
      wpi::util::to_string_view(message));
}

void HALSIM_SetEventName(const WPI_String* name) {
  SimDriverStationData->SetEventName(wpi::util::to_string_view(name));
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

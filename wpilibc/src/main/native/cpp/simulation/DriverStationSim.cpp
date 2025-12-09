// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DriverStationSim.hpp"

#include <memory>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/MockHooks.h"

using namespace wpi;
using namespace wpi::sim;

std::unique_ptr<CallbackStore> DriverStationSim::RegisterEnabledCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationEnabledCallback);
  store->SetUid(HALSIM_RegisterDriverStationEnabledCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DriverStationSim::GetEnabled() {
  return HALSIM_GetDriverStationEnabled();
}

void DriverStationSim::SetEnabled(bool enabled) {
  HALSIM_SetDriverStationEnabled(enabled);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterAutonomousCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationAutonomousCallback);
  store->SetUid(HALSIM_RegisterDriverStationAutonomousCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DriverStationSim::GetAutonomous() {
  return HALSIM_GetDriverStationAutonomous();
}

void DriverStationSim::SetAutonomous(bool autonomous) {
  HALSIM_SetDriverStationAutonomous(autonomous);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterTestCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationTestCallback);
  store->SetUid(HALSIM_RegisterDriverStationTestCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DriverStationSim::GetTest() {
  return HALSIM_GetDriverStationTest();
}

void DriverStationSim::SetTest(bool test) {
  HALSIM_SetDriverStationTest(test);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterEStopCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationEStopCallback);
  store->SetUid(HALSIM_RegisterDriverStationEStopCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DriverStationSim::GetEStop() {
  return HALSIM_GetDriverStationEStop();
}

void DriverStationSim::SetEStop(bool eStop) {
  HALSIM_SetDriverStationEStop(eStop);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterFmsAttachedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationFmsAttachedCallback);
  store->SetUid(HALSIM_RegisterDriverStationFmsAttachedCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DriverStationSim::GetFmsAttached() {
  return HALSIM_GetDriverStationFmsAttached();
}

void DriverStationSim::SetFmsAttached(bool fmsAttached) {
  HALSIM_SetDriverStationFmsAttached(fmsAttached);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterDsAttachedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationDsAttachedCallback);
  store->SetUid(HALSIM_RegisterDriverStationDsAttachedCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DriverStationSim::GetDsAttached() {
  return HALSIM_GetDriverStationDsAttached();
}

void DriverStationSim::SetDsAttached(bool dsAttached) {
  HALSIM_SetDriverStationDsAttached(dsAttached);
}

std::unique_ptr<CallbackStore>
DriverStationSim::RegisterAllianceStationIdCallback(NotifyCallback callback,
                                                    bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationAllianceStationIdCallback);
  store->SetUid(HALSIM_RegisterDriverStationAllianceStationIdCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

HAL_AllianceStationID DriverStationSim::GetAllianceStationId() {
  return HALSIM_GetDriverStationAllianceStationId();
}

void DriverStationSim::SetAllianceStationId(
    HAL_AllianceStationID allianceStationId) {
  HALSIM_SetDriverStationAllianceStationId(allianceStationId);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterMatchTimeCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelDriverStationMatchTimeCallback);
  store->SetUid(HALSIM_RegisterDriverStationMatchTimeCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double DriverStationSim::GetMatchTime() {
  return HALSIM_GetDriverStationMatchTime();
}

void DriverStationSim::SetMatchTime(double matchTime) {
  HALSIM_SetDriverStationMatchTime(matchTime);
}

void DriverStationSim::NotifyNewData() {
  wpi::util::Event waitEvent{true};
  HAL_ProvideNewDataEventHandle(waitEvent.GetHandle());
  HALSIM_NotifyDriverStationNewData();
  wpi::util::WaitForObject(waitEvent.GetHandle());
  HAL_RemoveNewDataEventHandle(waitEvent.GetHandle());
  wpi::DriverStation::RefreshData();
}

void DriverStationSim::SetSendError(bool shouldSend) {
  if (shouldSend) {
    HALSIM_SetSendError(nullptr);
  } else {
    HALSIM_SetSendError([](HAL_Bool isError, int32_t errorCode,
                           HAL_Bool isLVCode, const char* details,
                           const char* location, const char* callStack,
                           HAL_Bool printMsg) { return 0; });
  }
}

void DriverStationSim::SetSendConsoleLine(bool shouldSend) {
  if (shouldSend) {
    HALSIM_SetSendConsoleLine(nullptr);
  } else {
    HALSIM_SetSendConsoleLine([](const char* line) { return 0; });
  }
}

int32_t DriverStationSim::GetJoystickLeds(int stick) {
  int32_t leds = 0;
  HALSIM_GetJoystickLeds(stick, &leds);
  return leds;
}

int DriverStationSim::GetJoystickRumble(int stick, int rumbleNum) {
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
  int32_t leftTriggerRumble = 0;
  int32_t rightTriggerRumble = 0;
  HALSIM_GetJoystickRumbles(stick, &leftRumble, &rightRumble,
                            &leftTriggerRumble, &rightTriggerRumble);
  switch (rumbleNum) {
    case 0:
      return leftRumble;
    case 1:
      return rightRumble;
    case 2:
      return leftTriggerRumble;
    case 3:
      return rightTriggerRumble;
    default:
      return 0;
  }
}

void DriverStationSim::SetJoystickButton(int stick, int button, bool state) {
  HALSIM_SetJoystickButton(stick, button, state);
}

void DriverStationSim::SetJoystickAxis(int stick, int axis, double value) {
  HALSIM_SetJoystickAxis(stick, axis, value);
}

void DriverStationSim::SetJoystickPOV(int stick, int pov,
                                      DriverStation::POVDirection value) {
  HALSIM_SetJoystickPOV(stick, pov, static_cast<HAL_JoystickPOV>(value));
}

void DriverStationSim::SetJoystickAxesMaximumIndex(int stick,
                                                   int maximumIndex) {
  SetJoystickAxesAvailable(stick, (1 << maximumIndex) - 1);
}

void DriverStationSim::SetJoystickAxesAvailable(int stick, int count) {
  HALSIM_SetJoystickAxesAvailable(stick, count);
}

void DriverStationSim::SetJoystickPOVsMaximumIndex(int stick,
                                                   int maximumIndex) {
  SetJoystickPOVsAvailable(stick, (1 << maximumIndex) - 1);
}

void DriverStationSim::SetJoystickPOVsAvailable(int stick, int count) {
  HALSIM_SetJoystickPOVsAvailable(stick, count);
}

void DriverStationSim::SetJoystickButtonsMaximumIndex(int stick,
                                                      int maximumIndex) {
  if (maximumIndex >= 64) {
    SetJoystickButtonsAvailable(stick, 0xFFFFFFFFFFFFFFFFL);
  } else {
    SetJoystickButtonsAvailable(stick, (1L << maximumIndex) - 1);
  }
}

void DriverStationSim::SetJoystickButtonsAvailable(int stick,
                                                   uint64_t available) {
  HALSIM_SetJoystickButtonsAvailable(stick, available);
}

// void DriverStationSim::SetJoystickButtons(int stick, uint32_t buttons) {
//   HALSIM_SetJoystickButtonsValue(stick, buttons);
// }

// void DriverStationSim::SetJoystickAxisCount(int stick, int count) {
//   HALSIM_SetJoystickAxisCount(stick, count);
// }

// void DriverStationSim::SetJoystickPOVCount(int stick, int count) {
//   HALSIM_SetJoystickPOVCount(stick, count);
// }

// void DriverStationSim::SetJoystickButtonCount(int stick, int count) {
//   HALSIM_SetJoystickButtonCount(stick, count);
// }

void DriverStationSim::SetJoystickIsGamepad(int stick, bool isGamepad) {
  HALSIM_SetJoystickIsGamepad(stick, isGamepad);
}

void DriverStationSim::SetJoystickGamepadType(int stick, int type) {
  HALSIM_SetJoystickGamepadType(stick, type);
}

void DriverStationSim::SetJoystickSupportedOutputs(int stick,
                                                   int supportedOutputs) {
  HALSIM_SetJoystickSupportedOutputs(stick, supportedOutputs);
}

void DriverStationSim::SetJoystickName(int stick, std::string_view name) {
  auto str = wpi::util::make_string(name);
  HALSIM_SetJoystickName(stick, &str);
}

void DriverStationSim::SetGameSpecificMessage(std::string_view message) {
  auto str = wpi::util::make_string(message);
  HALSIM_SetGameSpecificMessage(&str);
}

void DriverStationSim::SetEventName(std::string_view name) {
  auto str = wpi::util::make_string(name);
  HALSIM_SetEventName(&str);
}

void DriverStationSim::SetMatchType(DriverStation::MatchType type) {
  HALSIM_SetMatchType(static_cast<HAL_MatchType>(static_cast<int>(type)));
}

void DriverStationSim::SetMatchNumber(int matchNumber) {
  HALSIM_SetMatchNumber(matchNumber);
}

void DriverStationSim::SetReplayNumber(int replayNumber) {
  HALSIM_SetReplayNumber(replayNumber);
}

void DriverStationSim::ResetData() {
  HALSIM_ResetDriverStationData();
}

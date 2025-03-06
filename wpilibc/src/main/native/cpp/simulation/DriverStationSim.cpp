// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DriverStationSim.h"

#include <memory>

#include <hal/DriverStation.h>
#include <hal/simulation/DriverStationData.h>
#include <hal/simulation/MockHooks.h>

#include "frc/DriverStation.h"

using namespace frc;
using namespace frc::sim;

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

std::unique_ptr<CallbackStore> DriverStationSim::RegisterOpModeCallback(
    OpModeCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(-1, callback,
                                               &HALSIM_CancelOpModeCallback);
  store->SetUid(HALSIM_RegisterOpModeCallback(&OpModeCallbackStoreThunk,
                                              store.get(), initialNotify));
  return store;
}

std::string DriverStationSim::GetOpMode() {
  WPI_String modeStr;
  HALSIM_GetOpMode(&modeStr);
  std::string mode{wpi::to_string_view(&modeStr)};
  WPI_FreeString(&modeStr);
  return mode;
}

void DriverStationSim::SetOpMode(std::string_view mode) {
  auto modeStr = wpi::make_string(mode);
  HALSIM_SetOpMode(&modeStr);
}

std::unique_ptr<CallbackStore>
DriverStationSim::RegisterSelectedAutonomousOpModeCallback(
    OpModeCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelSelectedAutonomousOpModeCallback);
  store->SetUid(HALSIM_RegisterSelectedAutonomousOpModeCallback(
      &OpModeCallbackStoreThunk, store.get(), initialNotify));
  return store;
}

std::string DriverStationSim::GetSelectedAutonomousOpMode() {
  WPI_String modeStr;
  HALSIM_GetSelectedAutonomousOpMode(&modeStr);
  std::string mode{wpi::to_string_view(&modeStr)};
  WPI_FreeString(&modeStr);
  return mode;
}

void DriverStationSim::SetSelectedAutonomousOpMode(std::string_view mode) {
  auto modeStr = wpi::make_string(mode);
  HALSIM_SetSelectedAutonomousOpMode(&modeStr);
}

std::unique_ptr<CallbackStore>
DriverStationSim::RegisterSelectedTeleoperatedOpModeCallback(
    OpModeCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelSelectedTeleoperatedOpModeCallback);
  store->SetUid(HALSIM_RegisterSelectedTeleoperatedOpModeCallback(
      &OpModeCallbackStoreThunk, store.get(), initialNotify));
  return store;
}

std::string DriverStationSim::GetSelectedTeleoperatedOpMode() {
  WPI_String modeStr;
  HALSIM_GetSelectedTeleoperatedOpMode(&modeStr);
  std::string mode{wpi::to_string_view(&modeStr)};
  WPI_FreeString(&modeStr);
  return mode;
}

void DriverStationSim::SetSelectedTeleoperatedOpMode(std::string_view mode) {
  auto modeStr = wpi::make_string(mode);
  HALSIM_SetSelectedTeleoperatedOpMode(&modeStr);
}

std::unique_ptr<CallbackStore> DriverStationSim::RegisterOpModeOptionsCallback(
    OpModeOptionsCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelOpModeOptionsCallback);
  store->SetUid(HALSIM_RegisterOpModeOptionsCallback(
      &OpModeOptionsCallbackStoreThunk, store.get(), initialNotify));
  return store;
}

std::vector<DriverStationSim::OpModeOption>
DriverStationSim::GetOpModeOptions() {
  int32_t len;
  HAL_OpModeOption* modesBuf = HALSIM_GetOpModeOptions(&len);
  std::vector<OpModeOption> modes;
  modes.reserve(len);
  for (int32_t i = 0; i < len; ++i) {
    modes.emplace_back(
        std::string{wpi::to_string_view(&modesBuf[i].name)},
        std::string{wpi::to_string_view(&modesBuf[i].category)},
        std::string{wpi::to_string_view(&modesBuf[i].description)},
        modesBuf[i].flags);
  }
  HALSIM_FreeOpModeOptions(modesBuf, len);
  return modes;
}

void DriverStationSim::NotifyNewData() {
  wpi::Event waitEvent{true};
  HAL_ProvideNewDataEventHandle(waitEvent.GetHandle());
  HALSIM_NotifyDriverStationNewData();
  wpi::WaitForObject(waitEvent.GetHandle());
  HAL_RemoveNewDataEventHandle(waitEvent.GetHandle());
  frc::DriverStation::RefreshData();
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

int64_t DriverStationSim::GetJoystickOutputs(int stick) {
  int64_t outputs = 0;
  int32_t leftRumble;
  int32_t rightRumble;
  HALSIM_GetJoystickOutputs(stick, &outputs, &leftRumble, &rightRumble);
  return outputs;
}

int DriverStationSim::GetJoystickRumble(int stick, int rumbleNum) {
  int64_t outputs;
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
  HALSIM_GetJoystickOutputs(stick, &outputs, &leftRumble, &rightRumble);
  return rumbleNum == 0 ? leftRumble : rightRumble;
}

void DriverStationSim::SetJoystickButton(int stick, int button, bool state) {
  HALSIM_SetJoystickButton(stick, button, state);
}

void DriverStationSim::SetJoystickAxis(int stick, int axis, double value) {
  HALSIM_SetJoystickAxis(stick, axis, value);
}

void DriverStationSim::SetJoystickPOV(int stick, int pov, int value) {
  HALSIM_SetJoystickPOV(stick, pov, value);
}

void DriverStationSim::SetJoystickButtons(int stick, uint32_t buttons) {
  HALSIM_SetJoystickButtonsValue(stick, buttons);
}

void DriverStationSim::SetJoystickAxisCount(int stick, int count) {
  HALSIM_SetJoystickAxisCount(stick, count);
}

void DriverStationSim::SetJoystickPOVCount(int stick, int count) {
  HALSIM_SetJoystickPOVCount(stick, count);
}

void DriverStationSim::SetJoystickButtonCount(int stick, int count) {
  HALSIM_SetJoystickButtonCount(stick, count);
}

void DriverStationSim::SetJoystickIsXbox(int stick, bool isXbox) {
  HALSIM_SetJoystickIsXbox(stick, isXbox);
}

void DriverStationSim::SetJoystickType(int stick, int type) {
  HALSIM_SetJoystickType(stick, type);
}

void DriverStationSim::SetJoystickName(int stick, std::string_view name) {
  auto str = wpi::make_string(name);
  HALSIM_SetJoystickName(stick, &str);
}

void DriverStationSim::SetJoystickAxisType(int stick, int axis, int type) {
  HALSIM_SetJoystickAxisType(stick, axis, type);
}

void DriverStationSim::SetGameSpecificMessage(std::string_view message) {
  auto str = wpi::make_string(message);
  HALSIM_SetGameSpecificMessage(&str);
}

void DriverStationSim::SetEventName(std::string_view name) {
  auto str = wpi::make_string(name);
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

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include <hal/simulation/DriverStationData.h>
#include <hal/simulation/MockHooks.h>

#include "CallbackStore.h"
#include "frc/DriverStation.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated driver station.
 */
class DriverStationSim {
 public:
  static std::unique_ptr<CallbackStore> RegisterEnabledCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationEnabledCallback);
    store->SetUid(HALSIM_RegisterDriverStationEnabledCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetEnabled() { return HALSIM_GetDriverStationEnabled(); }

  static void SetEnabled(bool enabled) {
    HALSIM_SetDriverStationEnabled(enabled);
  }

  static std::unique_ptr<CallbackStore> RegisterAutonomousCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationAutonomousCallback);
    store->SetUid(HALSIM_RegisterDriverStationAutonomousCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetAutonomous() { return HALSIM_GetDriverStationAutonomous(); }

  static void SetAutonomous(bool autonomous) {
    HALSIM_SetDriverStationAutonomous(autonomous);
  }

  static std::unique_ptr<CallbackStore> RegisterTestCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationTestCallback);
    store->SetUid(HALSIM_RegisterDriverStationTestCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetTest() { return HALSIM_GetDriverStationTest(); }

  static void SetTest(bool test) { HALSIM_SetDriverStationTest(test); }

  static std::unique_ptr<CallbackStore> RegisterEStopCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationEStopCallback);
    store->SetUid(HALSIM_RegisterDriverStationEStopCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetEStop() { return HALSIM_GetDriverStationEStop(); }

  static void SetEStop(bool eStop) { HALSIM_SetDriverStationEStop(eStop); }

  static std::unique_ptr<CallbackStore> RegisterFmsAttachedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationFmsAttachedCallback);
    store->SetUid(HALSIM_RegisterDriverStationFmsAttachedCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetFmsAttached() { return HALSIM_GetDriverStationFmsAttached(); }

  static void SetFmsAttached(bool fmsAttached) {
    HALSIM_SetDriverStationFmsAttached(fmsAttached);
  }

  static std::unique_ptr<CallbackStore> RegisterDsAttachedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationDsAttachedCallback);
    store->SetUid(HALSIM_RegisterDriverStationDsAttachedCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetDsAttached() { return HALSIM_GetDriverStationDsAttached(); }

  static void SetDsAttached(bool dsAttached) {
    HALSIM_SetDriverStationDsAttached(dsAttached);
  }

  static std::unique_ptr<CallbackStore> RegisterAllianceStationIdCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationAllianceStationIdCallback);
    store->SetUid(HALSIM_RegisterDriverStationAllianceStationIdCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static HAL_AllianceStationID GetAllianceStationId() {
    return HALSIM_GetDriverStationAllianceStationId();
  }

  static void SetAllianceStationId(HAL_AllianceStationID allianceStationId) {
    HALSIM_SetDriverStationAllianceStationId(allianceStationId);
  }

  static std::unique_ptr<CallbackStore> RegisterMatchTimeCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationMatchTimeCallback);
    store->SetUid(HALSIM_RegisterDriverStationMatchTimeCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetMatchTime() { return HALSIM_GetDriverStationMatchTime(); }

  static void SetMatchTime(double matchTime) {
    HALSIM_SetDriverStationMatchTime(matchTime);
  }

  /**
   * Updates DriverStation data so that new values are visible to the user
   * program.
   */
  static void NotifyNewData() {
    HALSIM_NotifyDriverStationNewData();
    DriverStation::GetInstance().WaitForData();
  }

  /**
   * Sets suppression of DriverStation::ReportError and ReportWarning messages.
   *
   * @param shouldSend If false then messages will be suppressed.
   */
  static void SetSendError(bool shouldSend) {
    if (shouldSend) {
      HALSIM_SetSendError(nullptr);
    } else {
      HALSIM_SetSendError([](HAL_Bool isError, int32_t errorCode,
                             HAL_Bool isLVCode, const char* details,
                             const char* location, const char* callStack,
                             HAL_Bool printMsg) { return 0; });
    }
  }

  /**
   * Sets suppression of DriverStation::SendConsoleLine messages.
   *
   * @param shouldSend If false then messages will be suppressed.
   */
  static void SetSendConsoleLine(bool shouldSend) {
    if (shouldSend) {
      HALSIM_SetSendConsoleLine(nullptr);
    } else {
      HALSIM_SetSendConsoleLine([](const char* line) { return 0; });
    }
  }

  /**
   * Gets the joystick outputs.
   *
   * @param stick The joystick number
   * @return The joystick outputs
   */
  static int64_t GetJoystickOutputs(int stick) {
    int64_t outputs = 0;
    int32_t leftRumble;
    int32_t rightRumble;
    HALSIM_GetJoystickOutputs(stick, &outputs, &leftRumble, &rightRumble);
    return outputs;
  }

  /**
   * Gets the joystick rumble.
   *
   * @param stick The joystick number
   * @param rumbleNum Rumble to get (0=left, 1=right)
   * @return The joystick rumble value
   */
  static int GetJoystickRumble(int stick, int rumbleNum) {
    int64_t outputs;
    int32_t leftRumble = 0;
    int32_t rightRumble = 0;
    HALSIM_GetJoystickOutputs(stick, &outputs, &leftRumble, &rightRumble);
    return rumbleNum == 0 ? leftRumble : rightRumble;
  }

  /**
   * Sets the state of one joystick button. Button indexes begin at 1.
   *
   * @param stick The joystick number
   * @param button The button index, beginning at 1
   * @param state The state of the joystick button
   */
  static void SetJoystickButton(int stick, int button, bool state) {
    HALSIM_SetJoystickButton(stick, button, state);
  }

  /**
   * Gets the value of the axis on a joystick.
   *
   * @param stick The joystick number
   * @param axis The analog axis number
   * @param value The value of the axis on the joystick
   */
  static void SetJoystickAxis(int stick, int axis, double value) {
    HALSIM_SetJoystickAxis(stick, axis, value);
  }

  /**
   * Gets the state of a POV on a joystick.
   *
   * @param stick The joystick number
   * @param pov The POV number
   * @param value the angle of the POV in degrees, or -1 for not pressed
   */
  static void SetJoystickPOV(int stick, int pov, int value) {
    HALSIM_SetJoystickPOV(stick, pov, value);
  }

  /**
   * Sets the state of all the buttons on a joystick.
   *
   * @param stick The joystick number
   * @param buttons The bitmap state of the buttons on the joystick
   */
  static void SetJoystickButtons(int stick, uint32_t buttons) {
    HALSIM_SetJoystickButtonsValue(stick, buttons);
  }

  /**
   * Sets the number of axes for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of axes on the indicated joystick
   */
  static void SetJoystickAxisCount(int stick, int count) {
    HALSIM_SetJoystickAxisCount(stick, count);
  }

  /**
   * Sets the number of POVs for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of POVs on the indicated joystick
   */
  static void SetJoystickPOVCount(int stick, int count) {
    HALSIM_SetJoystickPOVCount(stick, count);
  }

  /**
   * Sets the number of buttons for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of buttons on the indicated joystick
   */
  static void SetJoystickButtonCount(int stick, int count) {
    HALSIM_SetJoystickButtonCount(stick, count);
  }

  /**
   * Sets the value of isXbox for a joystick.
   *
   * @param stick The joystick number
   * @param isXbox The value of isXbox
   */
  static void SetJoystickIsXbox(int stick, bool isXbox) {
    HALSIM_SetJoystickIsXbox(stick, isXbox);
  }

  /**
   * Sets the value of type for a joystick.
   *
   * @param stick The joystick number
   * @param type The value of type
   */
  static void SetJoystickType(int stick, int type) {
    HALSIM_SetJoystickType(stick, type);
  }

  /**
   * Sets the name of a joystick.
   *
   * @param stick The joystick number
   * @param name The value of name
   */
  static void SetJoystickName(int stick, const char* name) {
    HALSIM_SetJoystickName(stick, name);
  }

  /**
   * Sets the types of Axes for a joystick.
   *
   * @param stick The joystick number
   * @param axis The target axis
   * @param type The type of axis
   */
  static void SetJoystickAxisType(int stick, int axis, int type) {
    HALSIM_SetJoystickAxisType(stick, axis, type);
  }

  /**
   * Sets the game specific message.
   *
   * @param message the game specific message
   */
  static void SetGameSpecificMessage(const char* message) {
    HALSIM_SetGameSpecificMessage(message);
  }

  /**
   * Sets the event name.
   *
   * @param name the event name
   */
  static void SetEventName(const char* name) { HALSIM_SetEventName(name); }

  /**
   * Sets the match type.
   *
   * @param type the match type
   */
  static void SetMatchType(DriverStation::MatchType type) {
    HALSIM_SetMatchType(static_cast<HAL_MatchType>(static_cast<int>(type)));
  }

  /**
   * Sets the match number.
   *
   * @param matchNumber the match number
   */
  static void SetMatchNumber(int matchNumber) {
    HALSIM_SetMatchNumber(matchNumber);
  }

  /**
   * Sets the replay number.
   *
   * @param replayNumber the replay number
   */
  static void SetReplayNumber(int replayNumber) {
    HALSIM_SetReplayNumber(replayNumber);
  }

  static void ResetData() { HALSIM_ResetDriverStationData(); }
};
}  // namespace sim
}  // namespace frc

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/CallbackStore.hpp"

namespace wpi::sim {

/**
 * Class to control a simulated driver station.
 */
class DriverStationSim {
 public:
  /**
   * Register a callback on whether the DS is enabled.
   *
   * @param callback the callback that will be called whenever the enabled
   *                 state is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterEnabledCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the DS is enabled.
   *
   * @return true if enabled
   */
  static bool GetEnabled();

  /**
   * Change whether the DS is enabled.
   *
   * @param enabled the new value
   */
  static void SetEnabled(bool enabled);

  /**
   * Register a callback on whether the DS is in autonomous mode.
   *
   * @param callback the callback that will be called on autonomous mode
   *                 entrance/exit
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterAutonomousCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the DS is in autonomous.
   *
   * @return true if autonomous
   */
  static bool GetAutonomous();

  /**
   * Change whether the DS is in autonomous.
   *
   * @param autonomous the new value
   */
  static void SetAutonomous(bool autonomous);

  /**
   * Register a callback on whether the DS is in test mode.
   *
   * @param callback the callback that will be called whenever the test mode
   *                 is entered or left
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterTestCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the DS is in test.
   *
   * @return true if test
   */
  static bool GetTest();

  /**
   * Change whether the DS is in test.
   *
   * @param test the new value
   */
  static void SetTest(bool test);

  /**
   * Register a callback on the eStop state.
   *
   * @param callback the callback that will be called whenever the eStop state
   *                 changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterEStopCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if eStop has been activated.
   *
   * @return true if eStopped
   */
  static bool GetEStop();

  /**
   * Set whether eStop is active.
   *
   * @param eStop true to activate
   */
  static void SetEStop(bool eStop);

  /**
   * Register a callback on whether the FMS is connected.
   *
   * @param callback the callback that will be called whenever the FMS
   *                 connection changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterFmsAttachedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the FMS is connected.
   *
   * @return true if FMS is connected
   */
  static bool GetFmsAttached();

  /**
   * Change whether the FMS is connected.
   *
   * @param fmsAttached the new value
   */
  static void SetFmsAttached(bool fmsAttached);

  /**
   * Register a callback on whether the DS is connected.
   *
   * @param callback the callback that will be called whenever the DS
   *                 connection changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterDsAttachedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the DS is attached.
   *
   * @return true if attached
   */
  static bool GetDsAttached();

  /**
   * Change whether the DS is attached.
   *
   * @param dsAttached the new value
   */
  static void SetDsAttached(bool dsAttached);

  /**
   * Register a callback on the alliance station ID.
   *
   * @param callback the callback that will be called whenever the alliance
   *                 station changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterAllianceStationIdCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the alliance station ID (color + number).
   *
   * @return the alliance station color and number
   */
  static HAL_AllianceStationID GetAllianceStationId();

  /**
   * Change the alliance station.
   *
   * @param allianceStationId the new alliance station
   */
  static void SetAllianceStationId(HAL_AllianceStationID allianceStationId);

  /**
   * Register a callback on match time.
   *
   * @param callback the callback that will be called whenever match time
   *                 changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterMatchTimeCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the current value of the match timer.
   *
   * @return the current match time
   */
  static double GetMatchTime();

  /**
   * Sets the match timer.
   *
   * @param matchTime the new match time
   */
  static void SetMatchTime(double matchTime);

  /**
   * Updates DriverStation data so that new values are visible to the user
   * program.
   */
  static void NotifyNewData();

  /**
   * Sets suppression of DriverStation::ReportError and ReportWarning messages.
   *
   * @param shouldSend If false then messages will be suppressed.
   */
  static void SetSendError(bool shouldSend);

  /**
   * Sets suppression of DriverStation::SendConsoleLine messages.
   *
   * @param shouldSend If false then messages will be suppressed.
   */
  static void SetSendConsoleLine(bool shouldSend);

  /**
   * Gets the joystick outputs.
   *
   * @param stick The joystick number
   * @return The joystick outputs
   */
  static int32_t GetJoystickLeds(int stick);

  /**
   * Gets the joystick rumble.
   *
   * @param stick The joystick number
   * @param rumbleNum Rumble to get (0=left, 1=right, 2=left trigger, 3=right
   * trigger)
   * @return The joystick rumble value
   */
  static int GetJoystickRumble(int stick, int rumbleNum);

  /**
   * Sets the state of one joystick button. %Button indexes begin at 0.
   *
   * @param stick The joystick number
   * @param button The button index, beginning at 0
   * @param state The state of the joystick button
   */
  static void SetJoystickButton(int stick, int button, bool state);

  /**
   * Gets the value of the axis on a joystick.
   *
   * @param stick The joystick number
   * @param axis The analog axis number
   * @param value The value of the axis on the joystick
   */
  static void SetJoystickAxis(int stick, int axis, double value);

  /**
   * Gets the state of a POV on a joystick.
   *
   * @param stick The joystick number
   * @param pov The POV number
   * @param value the angle of the POV
   */
  static void SetJoystickPOV(int stick, int pov,
                             DriverStation::POVDirection value);

  /**
   * Sets the number of axes for a joystick.
   *
   * @param stick The joystick number
   * @param maximumIndex The number of axes on the indicated joystick
   */
  static void SetJoystickAxesMaximumIndex(int stick, int maximumIndex);

  /**
   * Sets the number of axes for a joystick.
   *
   * @param stick The joystick number
   * @param available The number of axes on the indicated joystick
   */
  static void SetJoystickAxesAvailable(int stick, int available);

  /**
   * Sets the number of POVs for a joystick.
   *
   * @param stick The joystick number
   * @param maximumIndex The number of POVs on the indicated joystick
   */
  static void SetJoystickPOVsMaximumIndex(int stick, int maximumIndex);

  /**
   * Sets the number of POVs for a joystick.
   *
   * @param stick The joystick number
   * @param available The number of POVs on the indicated joystick
   */
  static void SetJoystickPOVsAvailable(int stick, int available);

  /**
   * Sets the number of buttons for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of buttons on the indicated joystick
   */
  static void SetJoystickButtonsMaximumIndex(int stick, int count);

  static void SetJoystickButtonsAvailable(int stick, uint64_t available);

  /**
   * Sets the value of isGamepad for a joystick.
   *
   * @param stick The joystick number
   * @param isGamepad The value of isGamepad
   */
  static void SetJoystickIsGamepad(int stick, bool isGamepad);

  /**
   * Sets the value of type for a joystick.
   *
   * @param stick The joystick number
   * @param type The value of type
   */
  static void SetJoystickGamepadType(int stick, int type);

  /**
   * Sets the name of a joystick.
   *
   * @param stick The joystick number
   * @param name The value of name
   */
  static void SetJoystickName(int stick, std::string_view name);

  /**
   * Sets the supported outputs for a joystick.
   *
   * @param stick The joystick number
   * @param supportedOutputs The supported outputs for the joystick
   */
  static void SetJoystickSupportedOutputs(int stick, int supportedOutputs);

  /**
   * Sets the game specific message.
   *
   * @param message the game specific message
   */
  static void SetGameSpecificMessage(std::string_view message);

  /**
   * Sets the event name.
   *
   * @param name the event name
   */
  static void SetEventName(std::string_view name);

  /**
   * Sets the match type.
   *
   * @param type the match type
   */
  static void SetMatchType(DriverStation::MatchType type);

  /**
   * Sets the match number.
   *
   * @param matchNumber the match number
   */
  static void SetMatchNumber(int matchNumber);

  /**
   * Sets the replay number.
   *
   * @param replayNumber the replay number
   */
  static void SetReplayNumber(int replayNumber);

  /**
   * Reset all simulation data for the Driver Station.
   */
  static void ResetData();
};
}  // namespace wpi::sim

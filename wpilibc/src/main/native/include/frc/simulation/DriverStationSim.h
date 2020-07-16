/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/DriverStationTypes.h>

#include "frc/DriverStation.h"
#include "frc/simulation/CallbackStore.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated driver station.
 */
class DriverStationSim {
 public:
  static std::unique_ptr<CallbackStore> RegisterEnabledCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetEnabled();

  static void SetEnabled(bool enabled);

  static std::unique_ptr<CallbackStore> RegisterAutonomousCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetAutonomous();

  static void SetAutonomous(bool autonomous);

  static std::unique_ptr<CallbackStore> RegisterTestCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetTest();

  static void SetTest(bool test);

  static std::unique_ptr<CallbackStore> RegisterEStopCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetEStop();

  static void SetEStop(bool eStop);

  static std::unique_ptr<CallbackStore> RegisterFmsAttachedCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetFmsAttached();

  static void SetFmsAttached(bool fmsAttached);

  static std::unique_ptr<CallbackStore> RegisterDsAttachedCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetDsAttached();

  static void SetDsAttached(bool dsAttached);

  static std::unique_ptr<CallbackStore> RegisterAllianceStationIdCallback(
      NotifyCallback callback, bool initialNotify);

  static HAL_AllianceStationID GetAllianceStationId();

  static void SetAllianceStationId(HAL_AllianceStationID allianceStationId);

  static std::unique_ptr<CallbackStore> RegisterMatchTimeCallback(
      NotifyCallback callback, bool initialNotify);

  static double GetMatchTime();

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
  static int64_t GetJoystickOutputs(int stick);

  /**
   * Gets the joystick rumble.
   *
   * @param stick The joystick number
   * @param rumbleNum Rumble to get (0=left, 1=right)
   * @return The joystick rumble value
   */
  static int GetJoystickRumble(int stick, int rumbleNum);

  /**
   * Sets the state of one joystick button. Button indexes begin at 1.
   *
   * @param stick The joystick number
   * @param button The button index, beginning at 1
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
   * @param value the angle of the POV in degrees, or -1 for not pressed
   */
  static void SetJoystickPOV(int stick, int pov, int value);

  /**
   * Sets the state of all the buttons on a joystick.
   *
   * @param stick The joystick number
   * @param buttons The bitmap state of the buttons on the joystick
   */
  static void SetJoystickButtons(int stick, uint32_t buttons);

  /**
   * Sets the number of axes for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of axes on the indicated joystick
   */
  static void SetJoystickAxisCount(int stick, int count);

  /**
   * Sets the number of POVs for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of POVs on the indicated joystick
   */
  static void SetJoystickPOVCount(int stick, int count);

  /**
   * Sets the number of buttons for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of buttons on the indicated joystick
   */
  static void SetJoystickButtonCount(int stick, int count);

  /**
   * Sets the value of isXbox for a joystick.
   *
   * @param stick The joystick number
   * @param isXbox The value of isXbox
   */
  static void SetJoystickIsXbox(int stick, bool isXbox);

  /**
   * Sets the value of type for a joystick.
   *
   * @param stick The joystick number
   * @param type The value of type
   */
  static void SetJoystickType(int stick, int type);

  /**
   * Sets the name of a joystick.
   *
   * @param stick The joystick number
   * @param name The value of name
   */
  static void SetJoystickName(int stick, const char* name);

  /**
   * Sets the types of Axes for a joystick.
   *
   * @param stick The joystick number
   * @param axis The target axis
   * @param type The type of axis
   */
  static void SetJoystickAxisType(int stick, int axis, int type);

  /**
   * Sets the game specific message.
   *
   * @param message the game specific message
   */
  static void SetGameSpecificMessage(const char* message);

  /**
   * Sets the event name.
   *
   * @param name the event name
   */
  static void SetEventName(const char* name);

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

  static void ResetData();
};
}  // namespace sim
}  // namespace frc

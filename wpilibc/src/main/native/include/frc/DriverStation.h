// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <units/time.h>
#include <wpi/deprecated.h>

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace frc {

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation {
 public:
  enum Alliance { kRed, kBlue, kInvalid };
  enum MatchType { kNone, kPractice, kQualification, kElimination };

  /**
   * Return a reference to the singleton DriverStation.
   *
   * @return Reference to the DS instance
   * @deprecated Use the static methods
   */
  WPI_DEPRECATED("Use static methods")
  static DriverStation& GetInstance();

  static constexpr int kJoystickPorts = 6;

  /**
   * The state of one joystick button. %Button indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return The state of the joystick button.
   */
  static bool GetStickButton(int stick, int button);

  /**
   * Whether one joystick button was pressed since the last check. %Button
   * indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was pressed since the last check.
   */
  static bool GetStickButtonPressed(int stick, int button);

  /**
   * Whether one joystick button was released since the last check. %Button
   * indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was released since the last check.
   */
  static bool GetStickButtonReleased(int stick, int button);

  /**
   * Get the value of the axis on a joystick.
   *
   * This depends on the mapping of the joystick connected to the specified
   * port.
   *
   * @param stick The joystick to read.
   * @param axis  The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick.
   */
  static double GetStickAxis(int stick, int axis);

  /**
   * Get the state of a POV on the joystick.
   *
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  static int GetStickPOV(int stick, int pov);

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  static int GetStickButtons(int stick);

  /**
   * Returns the number of axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of axes on the indicated joystick
   */
  static int GetStickAxisCount(int stick);

  /**
   * Returns the number of POVs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of POVs on the indicated joystick
   */
  static int GetStickPOVCount(int stick);

  /**
   * Returns the number of buttons on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of buttons on the indicated joystick
   */
  static int GetStickButtonCount(int stick);

  /**
   * Returns a boolean indicating if the controller is an xbox controller.
   *
   * @param stick The joystick port number
   * @return A boolean that is true if the controller is an xbox controller.
   */
  static bool GetJoystickIsXbox(int stick);

  /**
   * Returns the type of joystick at a given port.
   *
   * @param stick The joystick port number
   * @return The HID type of joystick at the given port
   */
  static int GetJoystickType(int stick);

  /**
   * Returns the name of the joystick at the given port.
   *
   * @param stick The joystick port number
   * @return The name of the joystick at the given port
   */
  static std::string GetJoystickName(int stick);

  /**
   * Returns the types of Axes on a given joystick port.
   *
   * @param stick The joystick port number and the target axis
   * @param axis  The analog axis value to read from the joystick.
   * @return What type of axis the axis is reporting to be
   */
  static int GetJoystickAxisType(int stick, int axis);

  /**
   * Returns if a joystick is connected to the Driver Station.
   *
   * This makes a best effort guess by looking at the reported number of axis,
   * buttons, and POVs attached.
   *
   * @param stick The joystick port number
   * @return true if a joystick is connected
   */
  static bool IsJoystickConnected(int stick);

  /**
   * Check if the DS has enabled the robot.
   *
   * @return True if the robot is enabled and the DS is connected
   */
  static bool IsEnabled();

  /**
   * Check if the robot is disabled.
   *
   * @return True if the robot is explicitly disabled or the DS is not connected
   */
  static bool IsDisabled();

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  static bool IsEStopped();

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  static bool IsAutonomous();

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  static bool IsAutonomousEnabled();

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   * @deprecated Use IsTeleop() instead.
   */
  WPI_DEPRECATED("Use IsTeleop() instead")
  static bool IsOperatorControl();

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  static bool IsTeleop();

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   * @deprecated Use IsTeleopEnabled() instead.
   */
  WPI_DEPRECATED("Use IsTeleopEnabled() instead")
  static bool IsOperatorControlEnabled();

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  static bool IsTeleopEnabled();

  /**
   * Check if the DS is commanding test mode.
   *
   * @return True if the robot is being commanded to be in test mode
   */
  static bool IsTest();

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  static bool IsDSAttached();

  /**
   * Has a new control packet from the driver station arrived since the last
   * time this function was called?
   *
   * Warning: If you call this function from more than one place at the same
   * time, you will not get the intended behavior.
   *
   * @return True if the control data has been updated since the last call.
   */
  static bool IsNewControlData();

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  static bool IsFMSAttached();

  /**
   * Returns the game specific message provided by the FMS.
   *
   * @return A string containing the game specific message.
   */
  static std::string GetGameSpecificMessage();

  /**
   * Returns the name of the competition event provided by the FMS.
   *
   * @return A string containing the event name
   */
  static std::string GetEventName();

  /**
   * Returns the type of match being played provided by the FMS.
   *
   * @return The match type enum (kNone, kPractice, kQualification,
   *         kElimination)
   */
  static MatchType GetMatchType();

  /**
   * Returns the match number provided by the FMS.
   *
   * @return The number of the match
   */
  static int GetMatchNumber();

  /**
   * Returns the number of times the current match has been replayed from the
   * FMS.
   *
   * @return The number of replays
   */
  static int GetReplayNumber();

  /**
   * Return the alliance that the driver station says it is on.
   *
   * This could return kRed or kBlue.
   *
   * @return The Alliance enum (kRed, kBlue or kInvalid)
   */
  static Alliance GetAlliance();

  /**
   * Return the driver station location on the field.
   *
   * This could return 1, 2, or 3.
   *
   * @return The location of the driver station (1-3, 0 for invalid)
   */
  static int GetLocation();

  /**
   * Wait until a new packet comes from the driver station.
   *
   * This blocks on a semaphore, so the waiting is efficient.
   *
   * This is a good way to delay processing until there is new driver station
   * data to act on.
   *
   * Checks if new control data has arrived since the last waitForData call
   * on the current thread. If new data has not arrived, returns immediately.
   */
  static void WaitForData();

  /**
   * Wait until a new packet comes from the driver station, or wait for a
   * timeout.
   *
   * Checks if new control data has arrived since the last waitForData call
   * on the current thread. If new data has not arrived, returns immediately.
   *
   * If the timeout is less then or equal to 0, wait indefinitely.
   *
   * Timeout is in milliseconds
   *
   * This blocks on a semaphore, so the waiting is efficient.
   *
   * This is a good way to delay processing until there is new driver station
   * data to act on.
   *
   * @param timeout Timeout
   *
   * @return true if new data, otherwise false
   */
  static bool WaitForData(units::second_t timeout);

  /**
   * Return the approximate match time.
   *
   * The FMS does not send an official match time to the robots, but does send
   * an approximate match time. The value will count down the time remaining in
   * the current period (auto or teleop).
   *
   * Warning: This is not an official time (so it cannot be used to dispute ref
   * calls or guarantee that a function will trigger before the match ends).
   *
   * The Practice Match function of the DS approximates the behavior seen on
   * the field.
   *
   * @return Time remaining in current match period (auto or teleop)
   */
  static double GetMatchTime();

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage in Volts.
   */
  static double GetBatteryVoltage();

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled
   *                 code.
   */
  static void InDisabled(bool entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving
   *                 autonomous code.
   */
  static void InAutonomous(bool entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop
   *                 code.
   * @deprecated Use InTeleop() instead.
   */
  WPI_DEPRECATED("Use InTeleop() instead")
  static void InOperatorControl(bool entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop
   *                 code.
   */
  static void InTeleop(bool entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code.
   */
  static void InTest(bool entering);

  /**
   * Forces WaitForData() to return immediately.
   */
  static void WakeupWaitForData();

  /**
   * Allows the user to specify whether they want joystick connection warnings
   * to be printed to the console. This setting is ignored when the FMS is
   * connected -- warnings will always be on in that scenario.
   *
   * @param silence Whether warning messages should be silenced.
   */
  static void SilenceJoystickConnectionWarning(bool silence);

  /**
   * Returns whether joystick connection warnings are silenced. This will
   * always return false when connected to the FMS.
   *
   * @return Whether joystick connection warnings are silenced.
   */
  static bool IsJoystickConnectionWarningSilenced();

  /**
   * Starts logging DriverStation data to data log. Repeated calls are ignored.
   *
   * @param log data log
   * @param logJoysticks if true, log joystick data
   */
  static void StartDataLog(wpi::log::DataLog& log, bool logJoysticks = true);

 private:
  DriverStation() = default;
};

}  // namespace frc

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include <hal/DriverStationTypes.h>
#include <wpi/Twine.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "frc/ErrorBase.h"

namespace frc {

class MatchDataSender;

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation : public ErrorBase {
 public:
  enum Alliance { kRed, kBlue, kInvalid };
  enum MatchType { kNone, kPractice, kQualification, kElimination };

  ~DriverStation() override;

  DriverStation(const DriverStation&) = delete;
  DriverStation& operator=(const DriverStation&) = delete;

  /**
   * Return a reference to the singleton DriverStation.
   *
   * @return Reference to the DS instance
   */
  static DriverStation& GetInstance();

  /**
   * Report an error to the DriverStation messages window.
   *
   * The error is also printed to the program console.
   */
  static void ReportError(const wpi::Twine& error);

  /**
   * Report a warning to the DriverStation messages window.
   *
   * The warning is also printed to the program console.
   */
  static void ReportWarning(const wpi::Twine& error);

  /**
   * Report an error to the DriverStation messages window.
   *
   * The error is also printed to the program console.
   */
  static void ReportError(bool isError, int code, const wpi::Twine& error,
                          const wpi::Twine& location, const wpi::Twine& stack);

  static constexpr int kJoystickPorts = 6;

  /**
   * The state of one joystick button. Button indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return The state of the joystick button.
   */
  bool GetStickButton(int stick, int button);

  /**
   * Whether one joystick button was pressed since the last check. Button
   * indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was pressed since the last check.
   */
  bool GetStickButtonPressed(int stick, int button);

  /**
   * Whether one joystick button was released since the last check. Button
   * indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was released since the last check.
   */
  bool GetStickButtonReleased(int stick, int button);

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
  double GetStickAxis(int stick, int axis);

  /**
   * Get the state of a POV on the joystick.
   *
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  int GetStickPOV(int stick, int pov);

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  int GetStickButtons(int stick) const;

  /**
   * Returns the number of axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of axes on the indicated joystick
   */
  int GetStickAxisCount(int stick) const;

  /**
   * Returns the number of POVs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of POVs on the indicated joystick
   */
  int GetStickPOVCount(int stick) const;

  /**
   * Returns the number of buttons on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of buttons on the indicated joystick
   */
  int GetStickButtonCount(int stick) const;

  /**
   * Returns a boolean indicating if the controller is an xbox controller.
   *
   * @param stick The joystick port number
   * @return A boolean that is true if the controller is an xbox controller.
   */
  bool GetJoystickIsXbox(int stick) const;

  /**
   * Returns the type of joystick at a given port.
   *
   * @param stick The joystick port number
   * @return The HID type of joystick at the given port
   */
  int GetJoystickType(int stick) const;

  /**
   * Returns the name of the joystick at the given port.
   *
   * @param stick The joystick port number
   * @return The name of the joystick at the given port
   */
  std::string GetJoystickName(int stick) const;

  /**
   * Returns the types of Axes on a given joystick port.
   *
   * @param stick The joystick port number and the target axis
   * @return What type of axis the axis is reporting to be
   */
  int GetJoystickAxisType(int stick, int axis) const;

  /**
   * Returns if a joystick is connected to the Driver Station.
   *
   * This makes a best effort guess by looking at the reported number of axis,
   * buttons, and POVs attached.
   *
   * @param stick The joystick port number
   * @return true if a joystick is connected
   */
  bool IsJoystickConnected(int stick) const;

  /**
   * Check if the DS has enabled the robot.
   *
   * @return True if the robot is enabled and the DS is connected
   */
  bool IsEnabled() const;

  /**
   * Check if the robot is disabled.
   *
   * @return True if the robot is explicitly disabled or the DS is not connected
   */
  bool IsDisabled() const;

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  bool IsEStopped() const;

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  bool IsAutonomous() const;

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  bool IsAutonomousEnabled() const;

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  bool IsOperatorControl() const;

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  bool IsOperatorControlEnabled() const;

  /**
   * Check if the DS is commanding test mode.
   *
   * @return True if the robot is being commanded to be in test mode
   */
  bool IsTest() const;

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  bool IsDSAttached() const;

  /**
   * Has a new control packet from the driver station arrived since the last
   * time this function was called?
   *
   * Warning: If you call this function from more than one place at the same
   * time, you will not get the intended behavior.
   *
   * @return True if the control data has been updated since the last call.
   */
  bool IsNewControlData() const;

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  bool IsFMSAttached() const;

  /**
   * Returns the game specific message provided by the FMS.
   *
   * @return A string containing the game specific message.
   */
  std::string GetGameSpecificMessage() const;

  /**
   * Returns the name of the competition event provided by the FMS.
   *
   * @return A string containing the event name
   */
  std::string GetEventName() const;

  /**
   * Returns the type of match being played provided by the FMS.
   *
   * @return The match type enum (kNone, kPractice, kQualification,
   *         kElimination)
   */
  MatchType GetMatchType() const;

  /**
   * Returns the match number provided by the FMS.
   *
   * @return The number of the match
   */
  int GetMatchNumber() const;

  /**
   * Returns the number of times the current match has been replayed from the
   * FMS.
   *
   * @return The number of replays
   */
  int GetReplayNumber() const;

  /**
   * Return the alliance that the driver station says it is on.
   *
   * This could return kRed or kBlue.
   *
   * @return The Alliance enum (kRed, kBlue or kInvalid)
   */
  Alliance GetAlliance() const;

  /**
   * Return the driver station location on the field.
   *
   * This could return 1, 2, or 3.
   *
   * @return The location of the driver station (1-3, 0 for invalid)
   */
  int GetLocation() const;

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
  void WaitForData();

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
   * @param timeout Timeout time in seconds
   *
   * @return true if new data, otherwise false
   */
  bool WaitForData(double timeout);

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
  double GetMatchTime() const;

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage in Volts.
   */
  double GetBatteryVoltage() const;

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled
   *                 code.
   */
  void InDisabled(bool entering) { m_userInDisabled = entering; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving
   *                 autonomous code.
   */
  void InAutonomous(bool entering) { m_userInAutonomous = entering; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop
   *                 code.
   */
  void InOperatorControl(bool entering) { m_userInTeleop = entering; }

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code.
   */
  void InTest(bool entering) { m_userInTest = entering; }

  /**
   * Forces WaitForData() to return immediately.
   */
  void WakeupWaitForData();

  /**
   * Allows the user to specify whether they want joystick connection warnings
   * to be printed to the console. This setting is ignored when the FMS is
   * connected -- warnings will always be on in that scenario.
   *
   * @param silence Whether warning messages should be silenced.
   */
  void SilenceJoystickConnectionWarning(bool silence);

  /**
   * Returns whether joystick connection warnings are silenced. This will
   * always return false when connected to the FMS.
   *
   * @return Whether joystick connection warnings are silenced.
   */
  bool IsJoystickConnectionWarningSilenced() const;

 protected:
  /**
   * Copy data from the DS task for the user.
   *
   * If no new data exists, it will just be returned, otherwise
   * the data will be copied from the DS polling loop.
   */
  void GetData();

 private:
  /**
   * DriverStation constructor.
   *
   * This is only called once the first time GetInstance() is called
   */
  DriverStation();

  /**
   * Reports errors related to unplugged joysticks.
   *
   * Throttles the errors so that they don't overwhelm the DS.
   */
  void ReportJoystickUnpluggedError(const wpi::Twine& message);

  /**
   * Reports errors related to unplugged joysticks.
   *
   * Throttles the errors so that they don't overwhelm the DS.
   */
  void ReportJoystickUnpluggedWarning(const wpi::Twine& message);

  void Run();

  void SendMatchData();

  std::unique_ptr<MatchDataSender> m_matchDataSender;

  // Joystick button rising/falling edge flags
  wpi::mutex m_buttonEdgeMutex;
  std::array<HAL_JoystickButtons, kJoystickPorts> m_previousButtonStates;
  std::array<uint32_t, kJoystickPorts> m_joystickButtonsPressed;
  std::array<uint32_t, kJoystickPorts> m_joystickButtonsReleased;

  // Internal Driver Station thread
  std::thread m_dsThread;
  std::atomic<bool> m_isRunning{false};

  mutable wpi::mutex m_waitForDataMutex;
  wpi::condition_variable m_waitForDataCond;
  int m_waitForDataCounter;

  bool m_silenceJoystickWarning = false;

  // Robot state status variables
  bool m_userInDisabled = false;
  bool m_userInAutonomous = false;
  bool m_userInTeleop = false;
  bool m_userInTest = false;

  double m_nextMessageTime = 0;
};

}  // namespace frc

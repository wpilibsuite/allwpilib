// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Synchronization.h"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace wpi {

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStation final {
 public:
  /**
   * The robot alliance that the robot is a part of.
   */
  enum Alliance {
    /// Red alliance.
    kRed,
    /// Blue alliance.
    kBlue
  };

  /**
   * The type of robot match that the robot is part of.
   */
  enum MatchType {
    /// None.
    kNone,
    /// Practice.
    kPractice,
    /// Qualification.
    kQualification,
    /// Elimination.
    kElimination
  };

  /**
   * A controller POV direction.
   */
  enum POVDirection : uint8_t {
    /// POV center.
    kCenter = HAL_JoystickPOV_kCentered,
    /// POV up.
    kUp = HAL_JoystickPOV_kUp,
    /// POV up right.
    kUpRight = HAL_JoystickPOV_kRightUp,
    /// POV right.
    kRight = HAL_JoystickPOV_kRight,
    /// POV down right.
    kDownRight = HAL_JoystickPOV_kRightDown,
    /// POV down.
    kDown = HAL_JoystickPOV_kDown,
    /// POV down left.
    kDownLeft = HAL_JoystickPOV_kLeftDown,
    /// POV left.
    kLeft = HAL_JoystickPOV_kLeft,
    /// POV up left.
    kUpLeft = HAL_JoystickPOV_kLeftUp,
  };

  /**
   * Gets the angle of a POVDirection.
   *
   * @param angle The POVDirection to convert.
   * @return The angle clockwise from straight up, or std::nullopt if the
   * POVDirection is kCenter.
   */
  static constexpr std::optional<wpi::math::Rotation2d> GetAngle(
      POVDirection angle) {
    switch (angle) {
      case kCenter:
        return std::nullopt;
      case kUp:
        return wpi::math::Rotation2d{0_deg};
      case kUpRight:
        return wpi::math::Rotation2d{45_deg};
      case kRight:
        return wpi::math::Rotation2d{90_deg};
      case kDownRight:
        return wpi::math::Rotation2d{135_deg};
      case kDown:
        return wpi::math::Rotation2d{180_deg};
      case kDownLeft:
        return wpi::math::Rotation2d{225_deg};
      case kLeft:
        return wpi::math::Rotation2d{270_deg};
      case kUpLeft:
        return wpi::math::Rotation2d{315_deg};
      default:
        return std::nullopt;
    }
  }

  /// Number of Joystick ports.
  static constexpr int kJoystickPorts = 6;

  /**
   * The state of one joystick button. Button indexes begin at 0.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 0.
   * @return The state of the joystick button.
   */
  static bool GetStickButton(int stick, int button);

  /**
   * The state of one joystick button, only if available. Button indexes begin
   * at 0.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 0.
   * @return The state of the joystick button, or empty if unavailable.
   */
  static std::optional<bool> GetStickButtonIfAvailable(int stick, int button);

  /**
   * Whether one joystick button was pressed since the last check. %Button
   * indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 0.
   * @return Whether the joystick button was pressed since the last check.
   */
  static bool GetStickButtonPressed(int stick, int button);

  /**
   * Whether one joystick button was released since the last check. %Button
   * indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 0.
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
   * Get the value of the axis on a joystick, if available.
   *
   * This depends on the mapping of the joystick connected to the specified
   * port.
   *
   * @param stick The joystick to read.
   * @param axis  The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick, or empty if not available.
   */
  static std::optional<double> GetStickAxisIfAvailable(int stick, int axis);

  /**
   * Get the state of a POV on the joystick.
   *
   * @return the angle of the POV.
   */
  static POVDirection GetStickPOV(int stick, int pov);

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  static uint64_t GetStickButtons(int stick);

  /**
   * Returns the maximum axis index on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The maximum axis index on the indicated joystick
   */
  static int GetStickAxesMaximumIndex(int stick);

  /**
   * Returns the mask of available axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The mask of available axes on the indicated joystick
   */
  static int GetStickAxesAvailable(int stick);

  /**
   * Returns the maximum POV index on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The maximum POV index on the indicated joystick
   */
  static int GetStickPOVsMaximumIndex(int stick);

  /**
   * Returns the mask of available POVs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The mask of available POVs on the indicated joystick
   */
  static int GetStickPOVsAvailable(int stick);

  /**
   * Returns the maximum button index on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The maximum button index on the indicated joystick
   */
  static int GetStickButtonsMaximumIndex(int stick);

  /**
   * Returns the mask of available buttons on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The mask of available buttons on the indicated joystick
   */
  static uint64_t GetStickButtonsAvailable(int stick);

  /**
   * Returns a boolean indicating if the controller is an xbox controller.
   *
   * @param stick The joystick port number
   * @return A boolean that is true if the controller is an xbox controller.
   */
  static bool GetJoystickIsGamepad(int stick);

  /**
   * Returns the type of joystick at a given port.
   *
   * This maps to SDL_GamepadType
   *
   * @param stick The joystick port number
   * @return The HID type of joystick at the given port
   */
  static int GetJoystickGamepadType(int stick);

  /**
   * Returns the number of outputs supported by the joystick at the given
   * port.
   *
   * @param stick The joystick port number
   * @return The number of outputs supported by the joystick at the given port
   */
  static int GetJoystickSupportedOutputs(int stick);

  /**
   * Returns the name of the joystick at the given port.
   *
   * @param stick The joystick port number
   * @return The name of the joystick at the given port
   */
  static std::string GetJoystickName(int stick);

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
   */
  static bool IsTeleop();

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
   * Check if the DS is commanding Test mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in Test mode and
   * enabled.
   */
  static bool IsTestEnabled();

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  static bool IsDSAttached();

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
   * If the FMS is not connected, it is set from the game data setting on the
   * driver station.
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
   * Get the current alliance from the FMS.
   *
   * If the FMS is not connected, it is set from the team alliance setting on
   * the driver station.
   *
   * @return The alliance (red or blue) or an empty optional if the alliance is
   * invalid
   */
  static std::optional<Alliance> GetAlliance();

  /**
   * Return the driver station location from the FMS.
   *
   * If the FMS is not connected, it is set from the team alliance setting on
   * the driver station.
   *
   * This could return 1, 2, or 3.
   *
   * @return The location of the driver station (1-3, 0 for invalid)
   */
  static std::optional<int> GetLocation();

  /**
   * Return the approximate match time. The FMS does not send an official match
   * time to the robots, but does send an approximate match time. The value will
   * count down the time remaining in the current period (auto or teleop).
   * Warning: This is not an official time (so it cannot be used to dispute ref
   * calls or guarantee that a function will trigger before the match ends).
   *
   * <p>When connected to the real field, this number only changes in full
   * integer increments, and always counts down.
   *
   * <p>When the DS is in practice mode, this number is a floating point number,
   * and counts down.
   *
   * <p>When the DS is in teleop or autonomous mode, this number is a floating
   * point number, and counts up.
   *
   * <p>Simulation matches DS behavior without an FMS connected.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  static wpi::units::second_t GetMatchTime();

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage in Volts.
   */
  static double GetBatteryVoltage();

  /**
   * Copy data from the DS task for the user. If no new data exists, it will
   * just be returned, otherwise the data will be copied from the DS polling
   * loop.
   */
  static void RefreshData();

  /**
   * Registers the given handle for DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  static void ProvideRefreshedDataEventHandle(WPI_EventHandle handle);

  /**
   * Unregisters the given handle from DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  static void RemoveRefreshedDataEventHandle(WPI_EventHandle handle);

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

}  // namespace wpi

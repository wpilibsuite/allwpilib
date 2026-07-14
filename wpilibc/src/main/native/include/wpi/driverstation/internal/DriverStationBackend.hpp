// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>

#include "wpi/driverstation/POVDirection.hpp"
#include "wpi/driverstation/TouchpadFinger.hpp"
#include "wpi/hal/DriverStationTypes.hpp"
#include "wpi/util/Synchronization.h"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace wpi {
class GenericHID;
}  // namespace wpi

namespace wpi::util {
class Color;
}  // namespace wpi::util

namespace wpi::internal {

using wpi::hal::RobotMode;

/**
 * Provide access to the network communication data to / from the Driver
 * Station.
 */
class DriverStationBackend final {
 public:
  /// Number of Joystick ports.
  static constexpr int JOYSTICK_PORTS = 6;

  /**
   * Constructs a GenericHID for the given port.
   *
   * @param port The port index on the Driver Station.
   * @return The GenericHID object for the given port.
   */
  static GenericHID ConstructGenericHID(int port);

  /**
   * Resets cached DriverStation HID wrapper objects.
   *
   * This is intended for test cleanup only. Any existing references to cached
   * GenericHID or Gamepad objects become invalid after this call.
   */
  static void ResetCachedHIDData();

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
   * Get the finger data of a touchpad on a joystick, if available.
   *
   * @param stick         The joystick to read.
   * @param touchpad The touchpad index to read from the joystick.
   * @param finger   The finger index to read from the touchpad.
   * @return The finger data of the touchpad on the joystick.
   */
  static TouchpadFinger GetStickTouchpadFinger(int stick, int touchpad,
                                               int finger);

  /**
   * Whether a finger on a touchpad is available.
   *
   *  @param stick         The joystick to read.
   * @param touchpad The touchpad index to read from the joystick.
   * @param finger   The finger index to read from the touchpad.
   * @return True if the finger data is available.
   */
  static bool GetStickTouchpadFingerAvailable(int stick, int touchpad,
                                              int finger);

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
  DriverStationBackend() = default;
};

}  // namespace wpi::internal

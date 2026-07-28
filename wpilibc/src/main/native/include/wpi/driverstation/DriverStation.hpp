// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/driverstation/internal/DriverStationBackend.hpp"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace wpi {

class Gamepad;
class GenericHID;

/**
 * Provides access to Driver Station functionality.
 */
class DriverStation final {
 public:
  DriverStation() = delete;

  /**
   * Gets the GenericHID object for the given port. GenericHID objects are
   * cached, so this will always return the same object for the same port.
   *
   * @param port The port index on the Driver Station that the controller is
   * plugged into (0-5).
   * @return The GenericHID object for the given port.
   */
  static GenericHID& GetGenericHID(int port);

  /**
   * Gets the Gamepad object for the given port. Gamepad objects are cached, so
   * this will always return the same object for the same port.
   *
   * @param port The port index on the Driver Station that the controller is
   * plugged into (0-5).
   * @return The Gamepad object for the given port.
   */
  static Gamepad& GetGamepad(int port);

  /**
   * Starts logging DriverStation data to data log, including joystick data.
   * Repeated calls are ignored.
   *
   * @param log data log
   */
  static void StartDataLog(wpi::log::DataLog& log) {
    wpi::internal::DriverStationBackend::StartDataLog(log);
  }

  /**
   * Starts logging DriverStation data to data log. Repeated calls are ignored.
   *
   * @param log data log
   * @param logJoysticks if true, log joystick data
   */
  static void StartDataLog(wpi::log::DataLog& log, bool logJoysticks) {
    wpi::internal::DriverStationBackend::StartDataLog(log, logJoysticks);
  }

  /**
   * Registers the given handle for DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  static void ProvideRefreshedDataEventHandle(WPI_EventHandle handle) {
    wpi::internal::DriverStationBackend::ProvideRefreshedDataEventHandle(
        handle);
  }

  /**
   * Unregisters the given handle from DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  static void RemoveRefreshedDataEventHandle(WPI_EventHandle handle) {
    wpi::internal::DriverStationBackend::RemoveRefreshedDataEventHandle(handle);
  }

 private:
  static GenericHID& GetGenericHIDUnderLock(int port);
};

}  // namespace wpi

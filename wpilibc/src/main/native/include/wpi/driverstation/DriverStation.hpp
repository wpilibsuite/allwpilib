// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/driverstation/internal/DriverStationBackend.hpp"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace wpi {

/**
 * Provides access to Driver Station functionality.
 */
class DriverStation final {
 public:
  DriverStation() = delete;

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
};

}  // namespace wpi

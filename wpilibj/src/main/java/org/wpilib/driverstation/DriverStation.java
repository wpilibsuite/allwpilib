// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.datalog.DataLog;
import org.wpilib.driverstation.internal.DriverStationBackend;

/** Provides access to Driver Station functionality. */
public final class DriverStation {
  private DriverStation() {}

  /**
   * Starts logging DriverStation data to data log, including joystick data. Repeated calls are
   * ignored.
   *
   * @param log data log
   */
  public static void startDataLog(DataLog log) {
    DriverStationBackend.startDataLog(log);
  }

  /**
   * Starts logging DriverStation data to data log. Repeated calls are ignored.
   *
   * @param log data log
   * @param logJoysticks if true, log joystick data
   */
  public static void startDataLog(DataLog log, boolean logJoysticks) {
    DriverStationBackend.startDataLog(log, logJoysticks);
  }

  /**
   * Registers the given handle for DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  public static void provideRefreshedDataEventHandle(int handle) {
    DriverStationBackend.provideRefreshedDataEventHandle(handle);
  }

  /**
   * Unregisters the given handle from DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  public static void removeRefreshedDataEventHandle(int handle) {
    DriverStationBackend.removeRefreshedDataEventHandle(handle);
  }
}

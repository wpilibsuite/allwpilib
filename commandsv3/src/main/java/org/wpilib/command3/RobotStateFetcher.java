// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.driverstation.RobotState;
import org.wpilib.hardware.hal.RobotMode;

/**
 * Helper class for fetching information about the state of the robot. This is a package-private
 * class so tests for this library don't need to hook into driverstation simulation and the HAL.
 */
abstract class RobotStateFetcher {
  private static volatile RobotStateFetcher s_fetcher;

  abstract long getOpModeId();

  abstract String getOpModeName();

  abstract RobotMode getRobotMode();

  /**
   * Gets the current fetcher implementation. If {@link #setFetcher(RobotStateFetcher)} has not
   * already been called to set an implementation, this will default to a {@link
   * DriverStationRobotStateFetcher} instance.
   *
   * @return The fetcher instance to use to get opmode information.
   */
  static RobotStateFetcher getFetcher() {
    // Default to pull from the DS unless otherwise specified
    if (s_fetcher == null) {
      synchronized (RobotStateFetcher.class) {
        if (s_fetcher == null) {
          s_fetcher = new DriverStationRobotStateFetcher();
        }
      }
    }

    return s_fetcher;
  }

  /**
   * Sets the fetcher to use. In tests, this is reset before every test with an implementation that
   * always returns an ID of 0 and an empty string for the opmode name.
   *
   * @param fetcher The fetcher implementation to set. Cannot be null.
   */
  static void setFetcher(RobotStateFetcher fetcher) {
    s_fetcher = requireNonNullParam(fetcher, "fetcher", "setFetcher");
  }

  static final class DriverStationRobotStateFetcher extends RobotStateFetcher {
    @Override
    public long getOpModeId() {
      return RobotState.getOpModeId();
    }

    @Override
    public String getOpModeName() {
      return RobotState.getOpMode();
    }

    @Override
    RobotMode getRobotMode() {
      return RobotState.getRobotMode();
    }
  }
}

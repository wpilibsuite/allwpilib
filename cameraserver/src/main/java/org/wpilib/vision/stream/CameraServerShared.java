// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.stream;

/** CameraServer shared functions. */
public interface CameraServerShared {
  /**
   * get the main thread id func.
   *
   * @return the robotMainThreadId
   */
  Long getRobotMainThreadId();

  /**
   * Report an error to the driver station.
   *
   * @param error the error to set
   */
  void reportDriverStationError(String error);

  /**
   * Get if running on a Systemcore.
   *
   * @return true if on Systemcore
   */
  default boolean isSystemcore() {
    return false;
  }
}

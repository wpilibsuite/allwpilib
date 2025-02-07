// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cameraserver;

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
   * Report usage.
   *
   * @param resource the resource name
   * @param data arbitrary string data
   */
  void reportUsage(String resource, String data);

  /**
   * Get if running on a roboRIO.
   *
   * @return true if on roboRIO
   */
  default boolean isRoboRIO() {
    return false;
  }
}

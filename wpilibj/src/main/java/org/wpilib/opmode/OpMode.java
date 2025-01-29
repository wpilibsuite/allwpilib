// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

/**
 * Top-level interface for opmode classes. Users should generally extend one of the abstract
 * implementations of this interface (e.g. {@link PeriodicOpMode} or {@link LinearOpMode}) rather
 * than directly implementing this interface.
 */
public interface OpMode {
  /**
   * This function is called periodically while the opmode is selected on the DS (robot is
   * disabled). Code that should only run once when the opmode is selected should go in the opmode
   * constructor.
   */
  default void disabledPeriodic() {}

  /**
   * This function is called when the opmode starts (robot is enabled).
   *
   * @param opModeId opmode unique ID
   * @throws InterruptedException when interrupted
   */
  void opModeRun(long opModeId) throws InterruptedException;

  /**
   * This function is called asynchronously when the robot is disabled, to request the opmode return
   * from opModeRun().
   */
  void opModeStop();

  /**
   * This function is called when the opmode is no longer selected on the DS or after opModeRun()
   * returns. The object will not be reused after this is called.
   */
  void opModeClose();
}

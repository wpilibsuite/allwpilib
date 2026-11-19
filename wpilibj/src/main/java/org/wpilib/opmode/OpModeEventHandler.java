// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

/**
 * Handles a lifecycle event of an opmode run by an {@link org.wpilib.framework.OpModeRobot}.
 *
 * <p>Handlers are registered with {@link
 * org.wpilib.framework.OpModeRobot#addOpModeSelectListener(OpModeEventHandler)}, {@link
 * org.wpilib.framework.OpModeRobot#addOpModeStartListener(OpModeEventHandler)}, {@link
 * org.wpilib.framework.OpModeRobot#addOpModeStopListener(OpModeEventHandler)}, and {@link
 * org.wpilib.framework.OpModeRobot#addOpModeCloseListener(OpModeEventHandler)}.
 *
 * <p>Handlers run synchronously on the robot loop thread, so they should return quickly.
 */
@FunctionalInterface
public interface OpModeEventHandler {
  /**
   * Handles the event.
   *
   * @param opModeName the name the opmode was registered under
   * @param opMode the opmode the event applies to
   */
  void accept(String opModeName, OpMode opMode);
}

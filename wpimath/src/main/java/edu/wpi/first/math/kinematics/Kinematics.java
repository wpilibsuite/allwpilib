// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolator;

/**
 * Helper class that converts a chassis velocity (dx and dtheta components) into wheel speeds. Robot
 * code should not use this directly- Instead, use the particular type for your drivetrain (e.g.,
 * {@link DifferentialDriveKinematics}).
 *
 * @param <S> The type of the wheel speeds.
 * @param <P> The type of the wheel positions.
 */
public interface Kinematics<S, P> extends Interpolator<P> {
  /**
   * Performs forward kinematics to return the resulting chassis speed from the wheel speeds. This
   * method is often used for odometry -- determining the robot's position on the field using data
   * from the real-world speed of each wheel on the robot.
   *
   * @param wheelSpeeds The speeds of the wheels.
   * @return The chassis speed.
   */
  ChassisSpeeds toChassisSpeeds(S wheelSpeeds);

  /**
   * Performs inverse kinematics to return the wheel speeds from a desired chassis velocity. This
   * method is often used to convert joystick values into wheel speeds.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @return The wheel speeds.
   */
  S toWheelSpeeds(ChassisSpeeds chassisSpeeds);

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given change in wheel
   * positions. This method is often used for odometry -- determining the robot's position on the
   * field using changes in the distance driven by each wheel on the robot.
   *
   * @param start The starting distances driven by the wheels.
   * @param end The ending distances driven by the wheels.
   * @return The resulting Twist2d in the robot's movement.
   */
  Twist2d toTwist2d(P start, P end);

  /**
   * Returns a copy of the wheel positions object.
   *
   * @param positions The wheel positions object to copy.
   * @return A copy.
   */
  P copy(P positions);

  /**
   * Copies the value of the wheel positions object into the output.
   *
   * @param positions The wheel positions object to copy. Will not be modified.
   * @param output The output variable of the copy operation. Will have the same value as the
   *     positions object after the call.
   */
  void copyInto(P positions, P output);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.interfaces;

import edu.wpi.first.math.geometry.Rotation3d;

/** Gyro that supports representation in the 3D space. */
public interface Gyro3D extends Gyro {
  /**
   * Return the Skew angle from the Gyroscope.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns clockwise. It needs to follow the NED
   * axis convention.
   *
   * @return skew angle.
   */
  double getRoll();

  /**
   * Return the Pitch angle from the Gyroscope.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns clockwise. It needs to follow the NED
   * axis convention.
   *
   * @return pitch angle.
   */
  double getPitch();

  /**
   * Return the Yaw angle from the Gyroscope.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns clockwise. It needs to follow the NED
   * axis convention.
   *
   * @return yaw angle.
   */
  double getYaw();

  /**
   * Return the heading of the robot as a {@link edu.wpi.first.math.geometry.Rotation3d}.
   *
   * @return the current heading of the robot as a {@link edu.wpi.first.math.geometry.Rotation3d}.
   */
  default Rotation3d getRotation3d() {
    return new Rotation3d(getRoll(), getPitch(), getYaw());
  }
}

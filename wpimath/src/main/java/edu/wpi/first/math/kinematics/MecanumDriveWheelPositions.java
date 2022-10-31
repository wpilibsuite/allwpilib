// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

public class MecanumDriveWheelPositions {
  /** Distance measured by the front left wheel. */
  public double frontLeftMeters;

  /** Distance measured by the front right wheel. */
  public double frontRightMeters;

  /** Distance measured by the rear left wheel. */
  public double rearLeftMeters;

  /** Distance measured by the rear right wheel. */
  public double rearRightMeters;

  /** Constructs a MecanumDriveWheelPositions with zeros for all member fields. */
  public MecanumDriveWheelPositions() {}

  /**
   * Constructs a MecanumDriveWheelPositions.
   *
   * @param frontLeftMeters Distance measured by the front left wheel.
   * @param frontRightMeters Distance measured by the front right wheel.
   * @param rearLeftMeters Distance measured by the rear left wheel.
   * @param rearRightMeters Distance measured by the rear right wheel.
   */
  public MecanumDriveWheelPositions(
      double frontLeftMeters,
      double frontRightMeters,
      double rearLeftMeters,
      double rearRightMeters) {
    this.frontLeftMeters = frontLeftMeters;
    this.frontRightMeters = frontRightMeters;
    this.rearLeftMeters = rearLeftMeters;
    this.rearRightMeters = rearRightMeters;
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveWheelPositions(Front Left: %.2f m, Front Right: %.2f m, "
            + "Rear Left: %.2f m, Rear Right: %.2f m)",
        frontLeftMeters, frontRightMeters, rearLeftMeters, rearRightMeters);
  }
}

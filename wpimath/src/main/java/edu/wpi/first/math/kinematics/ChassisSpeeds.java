// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.geometry.Rotation2d;

/**
 * Represents the speed of a robot chassis. Although this struct contains similar members compared
 * to a Twist2d, they do NOT represent the same thing. Whereas a Twist2d represents a change in pose
 * w.r.t to the robot frame of reference, this ChassisSpeeds struct represents a velocity w.r.t to
 * the robot frame of reference.
 *
 * <p>A strictly non-holonomic drivetrain, such as a differential drive, should never have a dy
 * component because it can never move sideways. Holonomic drivetrains such as swerve and mecanum
 * will often have all three components.
 */
public class ChassisSpeeds {
  /** Represents forward velocity w.r.t the robot frame of reference. (Fwd is +) */
  public double vxMetersPerSecond;

  /** Represents sideways velocity w.r.t the robot frame of reference. (Left is +) */
  public double vyMetersPerSecond;

  /** Represents the angular velocity of the robot frame. (CCW is +) */
  public double omegaRadiansPerSecond;

  /** Constructs a ChassisSpeeds with zeros for dx, dy, and theta. */
  public ChassisSpeeds() {}

  /**
   * Constructs a ChassisSpeeds object.
   *
   * @param vxMetersPerSecond Forward velocity.
   * @param vyMetersPerSecond Sideways velocity.
   * @param omegaRadiansPerSecond Angular velocity.
   */
  public ChassisSpeeds(
      double vxMetersPerSecond, double vyMetersPerSecond, double omegaRadiansPerSecond) {
    this.vxMetersPerSecond = vxMetersPerSecond;
    this.vyMetersPerSecond = vyMetersPerSecond;
    this.omegaRadiansPerSecond = omegaRadiansPerSecond;
  }

  /**
   * Converts a user provided field-relative set of speeds into a robot-relative ChassisSpeeds
   * object.
   *
   * @param vxMetersPerSecond The component of speed in the x direction relative to the field.
   *     Positive x is away from your alliance wall.
   * @param vyMetersPerSecond The component of speed in the y direction relative to the field.
   *     Positive y is to your left when standing behind the alliance wall.
   * @param omegaRadiansPerSecond The angular rate of the robot.
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisSpeeds object representing the speeds in the robot's frame of reference.
   */
  public static ChassisSpeeds fromFieldRelativeSpeeds(
      double vxMetersPerSecond,
      double vyMetersPerSecond,
      double omegaRadiansPerSecond,
      Rotation2d robotAngle) {
    return new ChassisSpeeds(
        vxMetersPerSecond * robotAngle.getCos() + vyMetersPerSecond * robotAngle.getSin(),
        -vxMetersPerSecond * robotAngle.getSin() + vyMetersPerSecond * robotAngle.getCos(),
        omegaRadiansPerSecond);
  }

  /**
   * Converts a user provided field-relative ChassisSpeeds object into a robot-relative
   * ChassisSpeeds object.
   *
   * @param fieldRelativeSpeeds The ChassisSpeeds object representing the speeds in the field frame
   *     of reference. Positive x is away from your alliance wall. Positive y is to your left when
   *     standing behind the alliance wall.
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisSpeeds object representing the speeds in the robot's frame of reference.
   */
  public static ChassisSpeeds fromFieldRelativeSpeeds(
      ChassisSpeeds fieldRelativeSpeeds, Rotation2d robotAngle) {
    return fromFieldRelativeSpeeds(
        fieldRelativeSpeeds.vxMetersPerSecond,
        fieldRelativeSpeeds.vyMetersPerSecond,
        fieldRelativeSpeeds.omegaRadiansPerSecond,
        robotAngle);
  }

  /**
   * Adds two ChassisSpeeds and returns the sum.
   * <p>For example, ChassisSpeeds{1.0, 0.5, 0.75} + ChassisSpeeds{2.0, 1.5, 0.25} = 
   * ChassisSpeeds{3.0, 2.0, 1.0}
   *
   * @param other The ChassisSpeeds to add.
   * @return The sum of the ChassisSpeeds.
   */
  public ChassisSpeeds plus(ChassisSpeeds other) {
    return new ChassisSpeeds(
        vxMetersPerSecond + other.vxMetersPerSecond,
        vyMetersPerSecond + other.vyMetersPerSecond, 
        omegaRadiansPerSecond + other.omegaRadiansPerSecond);
  }

  /**
   * Subtracts the other ChassisSpeeds from the other ChassisSpeeds and returns the difference.
   * <p>For example, ChassisSpeeds{5.0, 4.0, 2.0} - ChassisSpeeds{1.0, 2.0, 1.0} =
   * ChassisSpeeds{4.0, 2.0, 1.0}
   *
   * @param other The ChassisSpeeds to subtract.
   * @return The difference between the two ChassisSpeeds.
   */
  public ChassisSpeeds minus(ChassisSpeeds other) {
    return new ChassisSpeeds(
        vxMetersPerSecond - other.vxMetersPerSecond, 
        vyMetersPerSecond - other.vyMetersPerSecond,
        omegaRadiansPerSecond - other.omegaRadiansPerSecond);
  }

  /**
   * Returns the inverse of the current ChassisSpeeds.
   * This is equivalent to negating all components of the ChassisSpeeds.
   *
   * @return The inverse of the current ChassisSpeeds.
   */
  public ChassisSpeeds unaryMinus() {
    return new ChassisSpeeds(-vxMetersPerSecond, -vyMetersPerSecond, -omegaRadiansPerSecond);
  }

  /**
   * Multiplies the ChassisSpeeds by a scalar and returns the new ChassisSpeeds.
   * <p>For example, ChassisSpeeds{2.0, 2.5, 1.0} * 2 = ChassisSpeeds{4.0, 5.0, 1.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled ChassisSpeeds.
   */
  public ChassisSpeeds times(double scalar) {
    return new ChassisSpeeds(
      vxMetersPerSecond * scalar, 
      vyMetersPerSecond * scalar, 
      omegaRadiansPerSecond * scalar);
  }

  /**
   * Divides the ChassisSpeeds by a scalar and returns the new ChassisSpeeds.
   * <p>For example, ChassisSpeeds{2.0, 2.5, 1.0} / 2 = ChassisSpeeds{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to multiply by.
   * @return The reference to the new mutated object.
   */
  public ChassisSpeeds div(double scalar) {
    return new ChassisSpeeds(
      vxMetersPerSecond / scalar, 
      vyMetersPerSecond / scalar, 
      omegaRadiansPerSecond / scalar);
  }

  @Override
  public String toString() {
    return String.format(
        "ChassisSpeeds(Vx: %.2f m/s, Vy: %.2f m/s, Omega: %.2f rad/s)",
        vxMetersPerSecond, vyMetersPerSecond, omegaRadiansPerSecond);
  }
}

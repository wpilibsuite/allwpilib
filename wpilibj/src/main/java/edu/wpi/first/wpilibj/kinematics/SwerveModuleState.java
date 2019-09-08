/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import edu.wpi.first.wpilibj.geometry.Rotation2d;

/**
 * Represents the state of one swerve module.
 */
@SuppressWarnings("MemberName")
public class SwerveModuleState implements Comparable<SwerveModuleState> {

  /**
   * Speed of the wheel of the module.
   */
  public double speedMetersPerSecond;

  /**
   * Angle of the module.
   */
  public Rotation2d angle = Rotation2d.fromDegrees(0);

  /**
   * Constructs a SwerveModuleState with zeros for speed and angle.
   */
  public SwerveModuleState() {
  }

  /**
   * Constructs a SwerveModuleState.
   *
   * @param speedMetersPerSecond The speed of the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModuleState(double speedMetersPerSecond, Rotation2d angle) {
    this.speedMetersPerSecond = speedMetersPerSecond;
    this.angle = angle;
  }

  /**
   * Compares two swerve module states. One swerve module is "greater" than the other if its speed
   * is higher than the other.
   *
   * @param o The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  @SuppressWarnings("ParameterName")
  public int compareTo(SwerveModuleState o) {
    return Double.compare(this.speedMetersPerSecond, o.speedMetersPerSecond);
  }
}

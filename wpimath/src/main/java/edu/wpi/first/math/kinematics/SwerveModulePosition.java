// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.geometry.Rotation2d;
import java.util.Objects;

/** Represents the state of one swerve module. */
public class SwerveModulePosition implements Comparable<SwerveModulePosition> {
  /** Distance measured by the wheel of the module. */
  public double distanceMeters;

  /** Angle of the module. */
  public Rotation2d angle = Rotation2d.fromDegrees(0);

  /** Constructs a SwerveModulePosition with zeros for speed and angle. */
  public SwerveModulePosition() {}

  /**
   * Constructs a SwerveModulePosition.
   *
   * @param distanceMeters The distance measured by the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModulePosition(double distanceMeters, Rotation2d angle) {
    this.distanceMeters = distanceMeters;
    this.angle = angle;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof SwerveModulePosition) {
      return Double.compare(distanceMeters, ((SwerveModulePosition) obj).distanceMeters) == 0;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(distanceMeters);
  }

  /**
   * Compares two swerve module positions. One swerve module is "greater" than the other if its
   * speed is higher than the other.
   *
   * @param other The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  public int compareTo(SwerveModulePosition other) {
    return Double.compare(this.distanceMeters, other.distanceMeters);
  }

  @Override
  public String toString() {
    return String.format(
        "SwerveModulePosition(Distance: %.2f m/s, Angle: %s)", distanceMeters, angle);
  }
}

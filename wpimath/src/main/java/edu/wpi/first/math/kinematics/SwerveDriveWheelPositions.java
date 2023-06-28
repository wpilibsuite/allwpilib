// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import java.util.Arrays;
import java.util.Objects;
import java.util.function.BinaryOperator;

public class SwerveDriveWheelPositions implements WheelPositions<SwerveDriveWheelPositions> {
  public SwerveModulePosition[] positions;

  /**
   * Creates a new SwerveDriveWheelPositions instance.
   *
   * @param positions The swerve module positions. This will be deeply copied.
   */
  public SwerveDriveWheelPositions(SwerveModulePosition[] positions) {
    this.positions = new SwerveModulePosition[positions.length];
    for (int i = 0; i < positions.length; i++) {
      this.positions[i] = positions[i].copy();
    }
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof SwerveDriveWheelPositions) {
      SwerveDriveWheelPositions other = (SwerveDriveWheelPositions) obj;
      return Arrays.equals(this.positions, other.positions);
    }
    return false;
  }

  @Override
  public int hashCode() {
    // Cast to interpret positions as single argument, not array of the arguments
    return Objects.hash((Object) positions);
  }

  @Override
  public String toString() {
    return String.format("SwerveDriveWheelPositions(%s)", Arrays.toString(positions));
  }

  private SwerveDriveWheelPositions generate(
      SwerveDriveWheelPositions other, BinaryOperator<SwerveModulePosition> generator) {
    if (other.positions.length != positions.length) {
      throw new IllegalArgumentException("Inconsistent number of modules!");
    }
    var newPositions = new SwerveModulePosition[positions.length];
    for (int i = 0; i < positions.length; i++) {
      newPositions[i] = generator.apply(positions[i], other.positions[i]);
    }
    return new SwerveDriveWheelPositions(newPositions);
  }

  @Override
  public SwerveDriveWheelPositions copy() {
    return new SwerveDriveWheelPositions(positions);
  }

  @Override
  public SwerveDriveWheelPositions minus(SwerveDriveWheelPositions other) {
    return generate(other, (a, b) -> a.minus(b));
  }

  @Override
  public SwerveDriveWheelPositions interpolate(SwerveDriveWheelPositions endValue, double t) {
    return generate(endValue, (a, b) -> a.interpolate(b, t));
  }
}

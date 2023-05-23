package edu.wpi.first.math.kinematics;

import java.util.Arrays;
import java.util.Objects;
import java.util.function.BinaryOperator;
import java.util.function.IntFunction;

public class SwerveDriveWheelPositions implements WheelPositions<SwerveDriveWheelPositions> {
  public SwerveModulePosition[] positions;

  public SwerveDriveWheelPositions(SwerveModulePosition[] positions) {
    this.positions = positions;
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

  private SwerveDriveWheelPositions generate(IntFunction<SwerveModulePosition> generator) {
    var newPositions = new SwerveModulePosition[positions.length];
    for (int i = 0; i < positions.length; i++) {
      newPositions[i] = generator.apply(i);
    }
    return new SwerveDriveWheelPositions(newPositions);
  }

  private SwerveDriveWheelPositions generate(
      SwerveDriveWheelPositions other, BinaryOperator<SwerveModulePosition> generator) {
    if (other.positions.length != positions.length) {
      throw new IllegalArgumentException("Inconsistent number of modules!");
    }
    return generate(i -> generator.apply(positions[i], other.positions[i]));
  }

  @Override
  public SwerveDriveWheelPositions copy() {
    return generate(i -> positions[i].copy());
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

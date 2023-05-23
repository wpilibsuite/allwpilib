package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathUtil;
import java.util.Objects;

public class DifferentialDriveWheelPositions
    implements WheelPositions<DifferentialDriveWheelPositions> {
  /** Distance measured by the left side. */
  public double leftMeters;

  /** Distance measured by the right side. */
  public double rightMeters;

  /**
   * Constructs a DifferentialDriveWheelPositions.
   *
   * @param leftMeters Distance measured by the left side.
   * @param rightMeters Distance measured by the right side.
   */
  public DifferentialDriveWheelPositions(double leftMeters, double rightMeters) {
    this.leftMeters = leftMeters;
    this.rightMeters = rightMeters;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof DifferentialDriveWheelPositions) {
      DifferentialDriveWheelPositions other = (DifferentialDriveWheelPositions) obj;
      return Math.abs(other.leftMeters - leftMeters) < 1E-9
          && Math.abs(other.rightMeters - rightMeters) < 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(leftMeters, rightMeters);
  }

  @Override
  public String toString() {
    return String.format(
        "DifferentialDriveWheelPositions(Left: %.2f m, Right: %.2f m", leftMeters, rightMeters);
  }

  @Override
  public DifferentialDriveWheelPositions copy() {
    return new DifferentialDriveWheelPositions(leftMeters, rightMeters);
  }

  @Override
  public DifferentialDriveWheelPositions minus(DifferentialDriveWheelPositions other) {
    return new DifferentialDriveWheelPositions(
        this.leftMeters - other.leftMeters, this.rightMeters - other.rightMeters);
  }

  @Override
  public DifferentialDriveWheelPositions interpolate(
      DifferentialDriveWheelPositions endValue, double t) {
    return new DifferentialDriveWheelPositions(
        MathUtil.interpolate(this.leftMeters, endValue.leftMeters, t),
        MathUtil.interpolate(this.rightMeters, endValue.rightMeters, t));
  }
}

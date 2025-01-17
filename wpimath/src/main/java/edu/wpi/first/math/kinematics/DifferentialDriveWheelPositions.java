// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.proto.DifferentialDriveWheelPositionsProto;
import edu.wpi.first.math.kinematics.struct.DifferentialDriveWheelPositionsStruct;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the wheel positions for a differential drive drivetrain. */
public class DifferentialDriveWheelPositions
    implements StructSerializable,
        ProtobufSerializable,
        Interpolatable<DifferentialDriveWheelPositions> {
  /** Distance measured by the left side. */
  public double leftMeters;

  /** Distance measured by the right side. */
  public double rightMeters;

  /** DifferentialDriveWheelPositions struct for serialization. */
  public static final DifferentialDriveWheelPositionsStruct struct =
      new DifferentialDriveWheelPositionsStruct();

  /** DifferentialDriveWheelPositions struct for serialization. */
  public static final DifferentialDriveWheelPositionsProto proto =
      new DifferentialDriveWheelPositionsProto();

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

  /**
   * Constructs a DifferentialDriveWheelPositions.
   *
   * @param left Distance measured by the left side.
   * @param right Distance measured by the right side.
   */
  public DifferentialDriveWheelPositions(Distance left, Distance right) {
    this(left.in(Meters), right.in(Meters));
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DifferentialDriveWheelPositions other
        && Math.abs(other.leftMeters - leftMeters) < 1E-9
        && Math.abs(other.rightMeters - rightMeters) < 1E-9;
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
  public DifferentialDriveWheelPositions interpolate(
      DifferentialDriveWheelPositions endValue, double t) {
    return new DifferentialDriveWheelPositions(
        MathUtil.interpolate(this.leftMeters, endValue.leftMeters, t),
        MathUtil.interpolate(this.rightMeters, endValue.rightMeters, t));
  }
}

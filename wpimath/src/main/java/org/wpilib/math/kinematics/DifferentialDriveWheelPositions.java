// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static edu.wpi.first.units.Units.Meters;

import org.wpilib.math.util.MathUtil;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.DifferentialDriveWheelPositionsProto;
import org.wpilib.math.kinematics.struct.DifferentialDriveWheelPositionsStruct;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the wheel positions for a differential drive drivetrain. */
public class DifferentialDriveWheelPositions
    implements StructSerializable,
        ProtobufSerializable,
        Interpolatable<DifferentialDriveWheelPositions> {
  /** Distance measured by the left side in meters. */
  public double left;

  /** Distance measured by the right side in meters. */
  public double right;

  /** DifferentialDriveWheelPositions struct for serialization. */
  public static final DifferentialDriveWheelPositionsStruct struct =
      new DifferentialDriveWheelPositionsStruct();

  /** DifferentialDriveWheelPositions struct for serialization. */
  public static final DifferentialDriveWheelPositionsProto proto =
      new DifferentialDriveWheelPositionsProto();

  /**
   * Constructs a DifferentialDriveWheelPositions.
   *
   * @param left Distance measured by the left side in meters.
   * @param right Distance measured by the right side in meters.
   */
  public DifferentialDriveWheelPositions(double left, double right) {
    this.left = left;
    this.right = right;
  }

  /**
   * Constructs a DifferentialDriveWheelPositions.
   *
   * @param left Distance measured by the left side in meters.
   * @param right Distance measured by the right side in meters.
   */
  public DifferentialDriveWheelPositions(Distance left, Distance right) {
    this(left.in(Meters), right.in(Meters));
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DifferentialDriveWheelPositions other
        && Math.abs(other.left - left) < 1E-9
        && Math.abs(other.right - right) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(left, right);
  }

  @Override
  public String toString() {
    return String.format(
        "DifferentialDriveWheelPositions(Left: %.2f m, Right: %.2f m", left, right);
  }

  @Override
  public DifferentialDriveWheelPositions interpolate(
      DifferentialDriveWheelPositions endValue, double t) {
    return new DifferentialDriveWheelPositions(
        MathUtil.interpolate(this.left, endValue.left, t),
        MathUtil.interpolate(this.right, endValue.right, t));
  }
}

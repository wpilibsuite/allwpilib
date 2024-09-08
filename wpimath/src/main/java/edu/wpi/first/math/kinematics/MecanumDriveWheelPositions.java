// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.proto.MecanumDriveWheelPositionsProto;
import edu.wpi.first.math.kinematics.struct.MecanumDriveWheelPositionsStruct;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the wheel positions for a mecanum drive drivetrain. */
public class MecanumDriveWheelPositions
    implements Interpolatable<MecanumDriveWheelPositions>,
        ProtobufSerializable,
        StructSerializable {
  /** Distance measured by the front left wheel. */
  public double frontLeftMeters;

  /** Distance measured by the front right wheel. */
  public double frontRightMeters;

  /** Distance measured by the rear left wheel. */
  public double rearLeftMeters;

  /** Distance measured by the rear right wheel. */
  public double rearRightMeters;

  /** MecanumDriveWheelPositions protobuf for serialization. */
  public static final MecanumDriveWheelPositionsProto proto = new MecanumDriveWheelPositionsProto();

  /** MecanumDriveWheelPositions struct for serialization. */
  public static final MecanumDriveWheelPositionsStruct struct =
      new MecanumDriveWheelPositionsStruct();

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

  /**
   * Constructs a MecanumDriveWheelPositions.
   *
   * @param frontLeft Distance measured by the front left wheel.
   * @param frontRight Distance measured by the front right wheel.
   * @param rearLeft Distance measured by the rear left wheel.
   * @param rearRight Distance measured by the rear right wheel.
   */
  public MecanumDriveWheelPositions(
      Distance frontLeft, Distance frontRight, Distance rearLeft, Distance rearRight) {
    this(frontLeft.in(Meters), frontRight.in(Meters), rearLeft.in(Meters), rearRight.in(Meters));
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof MecanumDriveWheelPositions other
        && Math.abs(other.frontLeftMeters - frontLeftMeters) < 1E-9
        && Math.abs(other.frontRightMeters - frontRightMeters) < 1E-9
        && Math.abs(other.rearLeftMeters - rearLeftMeters) < 1E-9
        && Math.abs(other.rearRightMeters - rearRightMeters) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(frontLeftMeters, frontRightMeters, rearLeftMeters, rearRightMeters);
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveWheelPositions(Front Left: %.2f m, Front Right: %.2f m, "
            + "Rear Left: %.2f m, Rear Right: %.2f m)",
        frontLeftMeters, frontRightMeters, rearLeftMeters, rearRightMeters);
  }

  @Override
  public MecanumDriveWheelPositions interpolate(MecanumDriveWheelPositions endValue, double t) {
    return new MecanumDriveWheelPositions(
        MathUtil.interpolate(this.frontLeftMeters, endValue.frontLeftMeters, t),
        MathUtil.interpolate(this.frontRightMeters, endValue.frontRightMeters, t),
        MathUtil.interpolate(this.rearLeftMeters, endValue.rearLeftMeters, t),
        MathUtil.interpolate(this.rearRightMeters, endValue.rearRightMeters, t));
  }
}

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
  /** Distance measured by the front left wheel in meters. */
  public double frontLeft;

  /** Distance measured by the front right wheel in meters. */
  public double frontRight;

  /** Distance measured by the rear left wheel in meters. */
  public double rearLeft;

  /** Distance measured by the rear right wheel in meters. */
  public double rearRight;

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
   * @param frontLeft Distance measured by the front left wheel in meters.
   * @param frontRight Distance measured by the front right wheel in meters.
   * @param rearLeft Distance measured by the rear left wheel in meters.
   * @param rearRight Distance measured by the rear right wheel in meters.
   */
  public MecanumDriveWheelPositions(
      double frontLeft, double frontRight, double rearLeft, double rearRight) {
    this.frontLeft = frontLeft;
    this.frontRight = frontRight;
    this.rearLeft = rearLeft;
    this.rearRight = rearRight;
  }

  /**
   * Constructs a MecanumDriveWheelPositions.
   *
   * @param frontLeft Distance measured by the front left wheel in meters.
   * @param frontRight Distance measured by the front right wheel in meters.
   * @param rearLeft Distance measured by the rear left wheel in meters.
   * @param rearRight Distance measured by the rear right wheel in meters.
   */
  public MecanumDriveWheelPositions(
      Distance frontLeft, Distance frontRight, Distance rearLeft, Distance rearRight) {
    this(frontLeft.in(Meters), frontRight.in(Meters), rearLeft.in(Meters), rearRight.in(Meters));
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof MecanumDriveWheelPositions other
        && Math.abs(other.frontLeft - frontLeft) < 1E-9
        && Math.abs(other.frontRight - frontRight) < 1E-9
        && Math.abs(other.rearLeft - rearLeft) < 1E-9
        && Math.abs(other.rearRight - rearRight) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(frontLeft, frontRight, rearLeft, rearRight);
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveWheelPositions(Front Left: %.2f m, Front Right: %.2f m, "
            + "Rear Left: %.2f m, Rear Right: %.2f m)",
        frontLeft, frontRight, rearLeft, rearRight);
  }

  @Override
  public MecanumDriveWheelPositions interpolate(MecanumDriveWheelPositions endValue, double t) {
    return new MecanumDriveWheelPositions(
        MathUtil.lerp(this.frontLeft, endValue.frontLeft, t),
        MathUtil.lerp(this.frontRight, endValue.frontRight, t),
        MathUtil.lerp(this.rearLeft, endValue.rearLeft, t),
        MathUtil.lerp(this.rearRight, endValue.rearRight, t));
  }
}

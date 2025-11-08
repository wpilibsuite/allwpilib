// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.Meters;

import org.wpilib.math.util.MathUtil;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.SwerveModulePositionProto;
import org.wpilib.math.kinematics.struct.SwerveModulePositionStruct;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the state of one swerve module. */
public class SwerveModulePosition
    implements Comparable<SwerveModulePosition>,
        Interpolatable<SwerveModulePosition>,
        ProtobufSerializable,
        StructSerializable {
  /** Distance measured by the wheel of the module in meters. */
  public double distance;

  /** Angle of the module. */
  public Rotation2d angle = Rotation2d.kZero;

  /** SwerveModulePosition protobuf for serialization. */
  public static final SwerveModulePositionProto proto = new SwerveModulePositionProto();

  /** SwerveModulePosition struct for serialization. */
  public static final SwerveModulePositionStruct struct = new SwerveModulePositionStruct();

  /** Constructs a SwerveModulePosition with zeros for distance and angle. */
  public SwerveModulePosition() {}

  /**
   * Constructs a SwerveModulePosition.
   *
   * @param distance The distance measured by the wheel of the module in meters.
   * @param angle The angle of the module.
   */
  public SwerveModulePosition(double distance, Rotation2d angle) {
    this.distance = distance;
    this.angle = angle;
  }

  /**
   * Constructs a SwerveModulePosition.
   *
   * @param distance The distance measured by the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModulePosition(Distance distance, Rotation2d angle) {
    this(distance.in(Meters), angle);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof SwerveModulePosition other
        && Math.abs(other.distance - distance) < 1E-9
        && angle.equals(other.angle);
  }

  @Override
  public int hashCode() {
    return Objects.hash(distance, angle);
  }

  /**
   * Compares two swerve module positions. One swerve module is "greater" than the other if its
   * distance is higher than the other.
   *
   * @param other The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  public int compareTo(SwerveModulePosition other) {
    return Double.compare(this.distance, other.distance);
  }

  @Override
  public String toString() {
    return String.format("SwerveModulePosition(Distance: %.2f m, Angle: %s)", distance, angle);
  }

  /**
   * Returns a copy of this swerve module position.
   *
   * @return A copy.
   */
  public SwerveModulePosition copy() {
    return new SwerveModulePosition(distance, angle);
  }

  @Override
  public SwerveModulePosition interpolate(SwerveModulePosition endValue, double t) {
    return new SwerveModulePosition(
        MathUtil.lerp(this.distance, endValue.distance, t),
        this.angle.interpolate(endValue.angle, t));
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.proto.SwerveModuleStateProto;
import edu.wpi.first.math.kinematics.struct.SwerveModuleStateStruct;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the state of one swerve module. */
public class SwerveModuleState
    implements Comparable<SwerveModuleState>, ProtobufSerializable, StructSerializable {
  /** Speed of the wheel of the module in meters per second. */
  public double speed;

  /** Angle of the module. */
  public Rotation2d angle = Rotation2d.kZero;

  /** SwerveModuleState protobuf for serialization. */
  public static final SwerveModuleStateProto proto = new SwerveModuleStateProto();

  /** SwerveModuleState struct for serialization. */
  public static final SwerveModuleStateStruct struct = new SwerveModuleStateStruct();

  /** Constructs a SwerveModuleState with zeros for speed and angle. */
  public SwerveModuleState() {}

  /**
   * Constructs a SwerveModuleState.
   *
   * @param speed The speed of the wheel of the module in meters per second.
   * @param angle The angle of the module.
   */
  public SwerveModuleState(double speed, Rotation2d angle) {
    this.speed = speed;
    this.angle = angle;
  }

  /**
   * Constructs a SwerveModuleState.
   *
   * @param speed The speed of the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModuleState(LinearVelocity speed, Rotation2d angle) {
    this(speed.in(MetersPerSecond), angle);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof SwerveModuleState other
        && Math.abs(other.speed - speed) < 1E-9
        && angle.equals(other.angle);
  }

  @Override
  public int hashCode() {
    return Objects.hash(speed, angle);
  }

  /**
   * Compares two swerve module states. One swerve module is "greater" than the other if its speed
   * is higher than the other.
   *
   * @param other The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  public int compareTo(SwerveModuleState other) {
    return Double.compare(this.speed, other.speed);
  }

  @Override
  public String toString() {
    return String.format("SwerveModuleState(Speed: %.2f m/s, Angle: %s)", speed, angle);
  }

  /**
   * Minimize the change in heading this swerve module state would require by potentially reversing
   * the direction the wheel spins. If this is used with the PIDController class's continuous input
   * functionality, the furthest a wheel will ever rotate is 90 degrees.
   *
   * @param currentAngle The current module angle.
   * @return The optimized module state.
   */
  public SwerveModuleState optimize(Rotation2d currentAngle) {
    var delta = angle.minus(currentAngle);
    if (Math.abs(delta.getDegrees()) > 90.0) {
      return new SwerveModuleState(-speed, angle.rotateBy(Rotation2d.kPi));
    } else {
      return new SwerveModuleState(speed, angle);
    }
  }

  /**
   * Scales speed by cosine of angle error. This scales down movement perpendicular to the desired
   * direction of travel that can occur when modules change directions. This results in smoother
   * driving.
   *
   * @param currentAngle The current module angle.
   * @return The scaled module state.
   */
  public SwerveModuleState cosineScale(Rotation2d currentAngle) {
    return new SwerveModuleState(speed * angle.minus(currentAngle).getCos(), angle);
  }
}

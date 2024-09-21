// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.math.geometry.MutRotation2d;
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
  /** Speed of the wheel of the module. */
  private double speedMetersPerSecond;

  /** Angle of the module. */
  private final MutRotation2d angle = new MutRotation2d();

  /** SwerveModuleState protobuf for serialization. */
  public static final SwerveModuleStateProto proto = new SwerveModuleStateProto();

  /** SwerveModuleState struct for serialization. */
  public static final SwerveModuleStateStruct struct = new SwerveModuleStateStruct();

  /** Constructs a SwerveModuleState with zeros for speed and angle. */
  public SwerveModuleState() {}

  /**
   * Constructs a SwerveModuleState.
   *
   * @param speedMetersPerSecond The speed of the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModuleState(double speedMetersPerSecond, Rotation2d angle) {
    this.speedMetersPerSecond = speedMetersPerSecond;
    this.angle.mut_Rotation2d(angle);
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

  /**
   * Returns the angle of this SwerveModuleState.
   *
   * @return The angle of this SwerveModuleState.
   */
  public Rotation2d getAngle() {
    return angle;
  }

  /**
   * Returns the speed of this SwerveModuleState.
   *
   * @return The speed of this SwerveModuleState.
   */
  public double getSpeedMetersPerSecond() {
    return speedMetersPerSecond;
  }

  /**
   * Sets the angle of this SwerveModuleState.
   *
   * @param angle The new angle of this state.
   */
  public void setAngle(Rotation2d angle) {
    this.angle.mut_Rotation2d(angle);
  }

  /**
   * Sets the speed of this SwerveModuleState.
   *
   * @param speedMetersPerSecond The new speed of this state.
   */
  public void setSpeed(double speedMetersPerSecond) {
    this.speedMetersPerSecond = speedMetersPerSecond;
  }

  /**
   * Sets the angle and speed of this SwerveModuleState.
   *
   * @param speedMetersPerSecond The new speed of this state.
   * @param angle The new angle of this state.
   */
  public void setState(double speedMetersPerSecond, Rotation2d angle) {
    setAngle(angle);
    setSpeed(speedMetersPerSecond);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof SwerveModuleState other
        && Math.abs(other.speedMetersPerSecond - speedMetersPerSecond) < 1E-9
        && angle.equals(other.angle);
  }

  @Override
  public int hashCode() {
    return Objects.hash(speedMetersPerSecond, angle);
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
    return Double.compare(this.speedMetersPerSecond, other.speedMetersPerSecond);
  }

  @Override
  public String toString() {
    return String.format(
        "SwerveModuleState(Speed: %.2f m/s, Angle: %s)", speedMetersPerSecond, angle);
  }

  /**
   * Minimize the change in heading this swerve module state would require by potentially reversing
   * the direction the wheel spins. If this is used with the PIDController class's continuous input
   * functionality, the furthest a wheel will ever rotate is 90 degrees.
   *
   * @param currentModuleAngle The current module state's angle.
   */
  public void optimize(Rotation2d currentModuleAngle) {
    var delta = angle.deltaDegrees(currentModuleAngle);
    if (Math.abs(delta) > 90.0) {
      speedMetersPerSecond *= -1;
      angle.mut_rotateBy(Rotation2d.kPi);
    }
  }

  /**
   * Scales the speed of the module by the cosine of the angle error. This scales down movement
   * perpendicular to the desired direction of travel that can occur when modules change directions.
   * This results in smoother driver.
   *
   * @param currentModuleAngle The current module state's angle.
   */
  public void cosineScale(Rotation2d currentModuleAngle) {
    var delta = angle.deltaRadians(currentModuleAngle);
    var cosine = Math.cos(delta);
    speedMetersPerSecond *= cosine;
  }
}

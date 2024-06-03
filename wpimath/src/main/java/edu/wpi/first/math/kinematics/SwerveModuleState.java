// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.proto.SwerveModuleStateProto;
import edu.wpi.first.math.kinematics.struct.SwerveModuleStateStruct;
import edu.wpi.first.units.Distance;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the state of one swerve module. */
public class SwerveModuleState
    implements Comparable<SwerveModuleState>, ProtobufSerializable, StructSerializable {
  /** Speed of the wheel of the module. */
  public double speedMetersPerSecond;

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
   * @param speedMetersPerSecond The speed of the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModuleState(double speedMetersPerSecond, Rotation2d angle) {
    this.speedMetersPerSecond = speedMetersPerSecond;
    this.angle = angle;
  }

  /**
   * Constructs a SwerveModuleState.
   *
   * @param speed The speed of the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModuleState(Measure<Velocity<Distance>> speed, Rotation2d angle) {
    this(speed.in(MetersPerSecond), angle);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof SwerveModuleState) {
      SwerveModuleState other = (SwerveModuleState) obj;
      return Math.abs(other.speedMetersPerSecond - speedMetersPerSecond) < 1E-9
          && angle.equals(other.angle);
    }
    return false;
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
   * Minimize the change in heading the desired swerve module state would require by potentially
   * reversing the direction the wheel spins. If this is used with the PIDController class's
   * continuous input functionality, the furthest a wheel will ever rotate is 90 degrees.
   * <p>
   * Note: This method should not be used together with the {@link #cosineOptimization} method. 
   * Both methods perform angle optimization, and using them together is redundant.
   * 
   * @param desiredState The desired state.
   * @param currentAngle The current module angle.
   * @return Optimized swerve module state.
   */
  public static SwerveModuleState optimize(
      SwerveModuleState desiredState, Rotation2d currentAngle) {
    var delta = desiredState.angle.minus(currentAngle);
    if (Math.abs(delta.getDegrees()) > 90.0) {
      return new SwerveModuleState(
          -desiredState.speedMetersPerSecond, desiredState.angle.rotateBy(Rotation2d.kPi));
    } else {
      return new SwerveModuleState(desiredState.speedMetersPerSecond, desiredState.angle);
    }
  }

  /**
   * Scales the module speed by the cosine of the angle error. This reduces skew caused by changing direction.
   * <p>
   * For example, if the current angle of the module matches the desired angle (i.e., there is no error), 
   * the speed of the module remains unchanged as <strong>cos(0) = 1</strong>. However, if the current angle is 90 degrees off 
   * from the desired angle, the speed of the module becomes 0 as <strong>cos(90°) = 0</strong>. This means the module will stop 
   * moving, allowing it to correct its angle without moving in the wrong direction.
   * <p>
   * Note: This method should not be used together with the {@link #optimize} method.
   * Both methods perform angle optimization, and using them together is redundant.
   * 
   * @param desiredState The desired state.
   * @param currentAngle The current module angle.
   * @return The cosine compensated swerve module state.
   */
  public static SwerveModuleState cosineOptimization(
      SwerveModuleState desiredState, Rotation2d currentAngle) {
    var delta = desiredState.angle.minus(currentAngle);
    double speed = desiredState.speedMetersPerSecond * delta.getCos();

    if (Math.abs(delta.getDegrees()) > 90.0) {
      return new SwerveModuleState(speed, desiredState.angle.rotateBy(Rotation2d.kPi));
    } else {
      return new SwerveModuleState(speed, desiredState.angle);
    }
  }
}

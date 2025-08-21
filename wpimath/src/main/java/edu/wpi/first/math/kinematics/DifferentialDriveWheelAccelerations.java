// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;

import edu.wpi.first.math.kinematics.proto.DifferentialDriveWheelAccelerationsProto;
import edu.wpi.first.math.kinematics.struct.DifferentialDriveWheelAccelerationsStruct;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents the wheel accelerations for a differential drive drivetrain. */
public class DifferentialDriveWheelAccelerations implements ProtobufSerializable, StructSerializable {
  /** Acceleration of the left side of the robot in meters per second squared. */
  public double left;

  /** Acceleration of the right side of the robot in meters per second squared. */
  public double right;

  /** DifferentialDriveWheelAccelerations protobuf for serialization. */
  public static final DifferentialDriveWheelAccelerationsProto proto =
      new DifferentialDriveWheelAccelerationsProto();

  /** DifferentialDriveWheelAccelerations struct for serialization. */
  public static final DifferentialDriveWheelAccelerationsStruct struct =
      new DifferentialDriveWheelAccelerationsStruct();

  /** Constructs a DifferentialDriveWheelAccelerations with zeros for left and right accelerations. */
  public DifferentialDriveWheelAccelerations() {}

  /**
   * Constructs a DifferentialDriveWheelAccelerations.
   *
   * @param left The left acceleration in meters per second squared.
   * @param right The right acceleration in meters per second squared.
   */
  public DifferentialDriveWheelAccelerations(double left, double right) {
    this.left = left;
    this.right = right;
  }

  /**
   * Constructs a DifferentialDriveWheelAccelerations.
   *
   * @param left The left acceleration in meters per second squared.
   * @param right The right acceleration in meters per second squared.
   */
  public DifferentialDriveWheelAccelerations(LinearAcceleration left, LinearAcceleration right) {
    this(left.in(MetersPerSecondPerSecond), right.in(MetersPerSecondPerSecond));
  }

  /**
   * Adds two DifferentialDriveWheelAccelerations and returns the sum.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{1.0, 0.5} + DifferentialDriveWheelAccelerations{2.0, 1.5}
   * = DifferentialDriveWheelAccelerations{3.0, 2.0}
   *
   * @param other The DifferentialDriveWheelAccelerations to add.
   * @return The sum of the DifferentialDriveWheelAccelerations.
   */
  public DifferentialDriveWheelAccelerations plus(DifferentialDriveWheelAccelerations other) {
    return new DifferentialDriveWheelAccelerations(left + other.left, right + other.right);
  }

  /**
   * Subtracts the other DifferentialDriveWheelAccelerations from the current DifferentialDriveWheelAccelerations
   * and returns the difference.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{5.0, 4.0} - DifferentialDriveWheelAccelerations{1.0, 2.0}
   * = DifferentialDriveWheelAccelerations{4.0, 2.0}
   *
   * @param other The DifferentialDriveWheelAccelerations to subtract.
   * @return The difference between the two DifferentialDriveWheelAccelerations.
   */
  public DifferentialDriveWheelAccelerations minus(DifferentialDriveWheelAccelerations other) {
    return new DifferentialDriveWheelAccelerations(left - other.left, right - other.right);
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelAccelerations. This is equivalent to negating
   * all components of the DifferentialDriveWheelAccelerations.
   *
   * @return The inverse of the current DifferentialDriveWheelAccelerations.
   */
  public DifferentialDriveWheelAccelerations unaryMinus() {
    return new DifferentialDriveWheelAccelerations(-left, -right);
  }

  /**
   * Multiplies the DifferentialDriveWheelAccelerations by a scalar and returns the new
   * DifferentialDriveWheelAccelerations.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{2.0, 2.5} * 2 = DifferentialDriveWheelAccelerations{4.0,
   * 5.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled DifferentialDriveWheelAccelerations.
   */
  public DifferentialDriveWheelAccelerations times(double scalar) {
    return new DifferentialDriveWheelAccelerations(left * scalar, right * scalar);
  }

  /**
   * Divides the DifferentialDriveWheelAccelerations by a scalar and returns the new
   * DifferentialDriveWheelAccelerations.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{2.0, 2.5} / 2 = DifferentialDriveWheelAccelerations{1.0,
   * 1.25}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled DifferentialDriveWheelAccelerations.
   */
  public DifferentialDriveWheelAccelerations div(double scalar) {
    return new DifferentialDriveWheelAccelerations(left / scalar, right / scalar);
  }

  @Override
  public String toString() {
    return String.format(
        "DifferentialDriveWheelAccelerations(Left: %.2f m/s², Right: %.2f m/s²)", left, right);
  }
}

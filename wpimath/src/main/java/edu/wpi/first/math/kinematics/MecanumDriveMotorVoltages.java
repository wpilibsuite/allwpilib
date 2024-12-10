// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.kinematics.proto.MecanumDriveMotorVoltagesProto;
import edu.wpi.first.math.kinematics.struct.MecanumDriveMotorVoltagesStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents the motor voltages for a mecanum drive drivetrain. */
public class MecanumDriveMotorVoltages implements ProtobufSerializable, StructSerializable {
  /** Voltage of the front left motor. */
  public double frontLeft;

  /** Voltage of the front right motor. */
  public double frontRight;

  /** Voltage of the rear left motor. */
  public double rearLeft;

  /** Voltage of the rear right motor. */
  public double rearRight;

  /** Constructs a MecanumDriveMotorVoltages with zeros for all member fields. */
  public MecanumDriveMotorVoltages() {}

  /**
   * Constructs a MecanumDriveMotorVoltages.
   *
   * @param frontLeft Voltage of the front left motor.
   * @param frontRight Voltage of the front right motor.
   * @param rearLeft Voltage of the rear left motor.
   * @param rearRight Voltage of the rear right motor.
   */
  public MecanumDriveMotorVoltages(
      double frontLeft, double frontRight, double rearLeft, double rearRight) {
    this.frontLeft = frontLeft;
    this.frontRight = frontRight;
    this.rearLeft = rearLeft;
    this.rearRight = rearRight;
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveMotorVoltages(Front Left: %.2f V, Front Right: %.2f V, "
            + "Rear Left: %.2f V, Rear Right: %.2f V)",
        frontLeft, frontRight, rearLeft, rearRight);
  }

  /** MecanumDriveMotorVoltages struct for serialization. */
  public static final MecanumDriveMotorVoltagesStruct struct =
      new MecanumDriveMotorVoltagesStruct();

  /** MecanumDriveMotorVoltages protobuf for serialization. */
  public static final MecanumDriveMotorVoltagesProto proto = new MecanumDriveMotorVoltagesProto();
}

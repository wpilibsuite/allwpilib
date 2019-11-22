/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

/**
 * Represents the motor voltages for a mecanum drive drivetrain.
 */
@SuppressWarnings("MemberName")
public class MecanumDriveMotorVoltages {
  /**
   * Voltage of the front left motor.
   */
  public double frontLeftVoltage;

  /**
   * Voltage of the front right motor.
   */
  public double frontRightVoltage;

  /**
   * Voltage of the rear left motor.
   */
  public double rearLeftVoltage;

  /**
   * Voltage of the rear right motor.
   */
  public double rearRightVoltage;

  /**
   * Constructs a MecanumDriveMotorVoltages with zeros for all member fields.
   */
  public MecanumDriveMotorVoltages() {
  }

  /**
   * Constructs a MecanumDriveMotorVoltages.
   *
   * @param frontLeftVoltage  Voltage of the front left motor.
   * @param frontRightVoltage Voltage of the front right motor.
   * @param rearLeftVoltage   Voltage of the rear left motor.
   * @param rearRightVoltage  Voltage of the rear right motor.
   */
  public MecanumDriveMotorVoltages(double frontLeftVoltage,
                                 double frontRightVoltage,
                                 double rearLeftVoltage,
                                 double rearRightVoltage) {
    this.frontLeftVoltage = frontLeftVoltage;
    this.frontRightVoltage = frontRightVoltage;
    this.rearLeftVoltage = rearLeftVoltage;
    this.rearRightVoltage = rearRightVoltage;
  }
}

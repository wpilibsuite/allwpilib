// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.controlstutorialexamples;

import org.wpilib.math.util.Units;

public class Constants {
  public static final int kJoystickPort = 0;

  // Flywheel constants
  public static final int kFlywheelMotorPort = 0;
  public static final int kFlywheelEncoderAChannel = 0;
  public static final int kFlywheelEncoderBChannel = 1;
  public static final double kFlywheelKp = 0.0001;
  public static final double kFlywheelKi = 0.0;
  public static final double kFlywheelKd = 0.0;
  public static final double kFlywheelKf = 0.000175;
  public static final double kFlywheelGearing = 1.0;
  public static final double kFlywheelMomentOfInertia = 0.00032; // kg * m^2
  public static final double kFlywheelEncoderDistPerPulse = 2.0 * Math.PI / 2048.0;
  public static final double kFlywheelSetpoint1 = 4000.0; // RPM
  public static final double kFlywheelSetpoint2 = 2000.0; // RPM

  // Turret constants
  public static final int kTurretMotorPort = 1;
  public static final int kTurretEncoderAChannel = 2;
  public static final int kTurretEncoderBChannel = 3;
  public static final double kTurretKp = 5.0;
  public static final double kTurretKi = 0.0;
  public static final double kTurretKd = 0.0;
  public static final double kTurretKf = 0.0;
  public static final double kTurretGearing = 300.0;
  public static final double kTurretMomentOfInertia = 0.005; // kg * m^2
  public static final double kTurretEncoderDistPerPulse = 2.0 * Math.PI / 2048.0;
  public static final double kTurretSetpoint1 = Units.degreesToRadians(45.0);
  public static final double kTurretSetpoint2 = Units.degreesToRadians(-45.0);

  // Vertical Arm constants
  public static final int kArmMotorPort = 2;
  public static final int kArmEncoderAChannel = 4;
  public static final int kArmEncoderBChannel = 5;
  public static final double kArmKp = 5.0;
  public static final double kArmKi = 0.0;
  public static final double kArmKd = 0.0;
  public static final double kArmKf = 0.0;
  public static final double kArmGearing = 200.0;
  public static final double kArmMomentOfInertia = 0.01; // kg * m^2
  public static final double kArmEncoderDistPerPulse = 2.0 * Math.PI / 2048.0;
  public static final double kArmSetpoint1 = Units.degreesToRadians(90.0);
  public static final double kArmSetpoint2 = Units.degreesToRadians(0.0);

  // Elevator constants
  public static final int kElevatorMotorPort = 3;
  public static final int kElevatorEncoderAChannel = 6;
  public static final int kElevatorEncoderBChannel = 7;
  public static final double kElevatorKp = 5.0;
  public static final double kElevatorKi = 0.0;
  public static final double kElevatorKd = 0.0;
  public static final double kElevatorKf = 0.0;
  public static final double kElevatorGearing = 10.0;
  public static final double kElevatorDrumRadius = Units.inchesToMeters(2.0);
  public static final double kElevatorCarriageMass = 4.0; // kg
  public static final double kElevatorMinHeight = 0.0; // m
  public static final double kElevatorMaxHeight = 1.25; // m
  public static final double kElevatorEncoderDistPerPulse =
      2.0 * Math.PI * kElevatorDrumRadius / 4096.0;
  public static final double kElevatorSetpoint1 = 1.0; // m
  public static final double kElevatorSetpoint2 = 0.0; // m

  // Timing
  public static final double kSetpointToggleTime = 5.0; // seconds
}
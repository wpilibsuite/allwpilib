// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.elevatorsimulation;

import edu.wpi.first.math.util.Units;

public class Constants {
  public static final int kMotorPort = 0;
  public static final int kEncoderAChannel = 0;
  public static final int kEncoderBChannel = 1;
  public static final int kJoystickPort = 0;

  public static final double kElevatorKp = 5;
  public static final double kElevatorKi = 0;
  public static final double kElevatorKd = 0;

  public static final double kElevatorkS = 0.0; // volts (V)
  public static final double kElevatorkG = 0.762; // volts (V)
  public static final double kElevatorkV = 0.762; // volt per velocity (V/(m/s))
  public static final double kElevatorkA = 0.0; // volt per acceleration (V/(m/s²))

  public static final double kElevatorGearing = 10.0;
  public static final double kElevatorDrumRadius = Units.inchesToMeters(2.0);
  public static final double kCarriageMass = 4.0; // kg

  public static final double kSetpoint = 0.75; // m
  // Encoder is reset to measure 0 at the bottom, so minimum height is 0.
  public static final double kMinElevatorHeight = 0.0; // m
  public static final double kMaxElevatorHeight = 1.25; // m

  // distance per pulse = (distance per revolution) / (pulses per revolution)
  //  = (Pi * D) / ppr
  public static final double kElevatorEncoderDistPerPulse =
      2.0 * Math.PI * kElevatorDrumRadius / 4096;
}

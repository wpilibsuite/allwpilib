// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.elevatorexponentialsimulation;

import edu.wpi.first.math.util.Units;

public class Constants {
  public static final int MOTOR_PORT = 0;
  public static final int ENCODER_A_CHANNEL = 0;
  public static final int ENCODER_B_CHANNEL = 1;
  public static final int JOYSTICK_PORT = 0;

  public static final double ELEVATOR_P = 0.75;
  public static final double ELEVATOR_I = 0;
  public static final double ELEVATOR_D = 0;

  public static final double ELEVATOR_MAX_V = 10.0; // volts (V)
  public static final double ELEVATOR_S = 0.0; // volts (V)
  public static final double ELEVATOR_G = 0.62; // volts (V)
  public static final double ELEVATOR_V = 3.9; // volts (V)
  public static final double ELEVATOR_A = 0.06; // volts (V)

  public static final double ELEVATOR_GEARING = 5.0;
  public static final double ELEVATOR_DRUM_RADIUS = Units.inchesToMeters(1.0);
  public static final double CARRIAGE_MASS = Units.lbsToKilograms(12); // kg

  public static final double SETPOINT = Units.inchesToMeters(42.875);
  public static final double LOWER_SETPOINT = Units.inchesToMeters(15);
  // Encoder is reset to measure 0 at the bottom, so minimum height is 0.
  public static final double MIN_ELEVATOR_HEIGHT = 0.0; // m
  public static final double MAX_ELEVATOR_HEIGHT = Units.inchesToMeters(50);

  // distance per pulse = (distance per revolution) / (pulses per revolution)
  //  = (Pi * D) / ppr
  public static final double ELEVATOR_ENCODER_DIST_PER_PULSE =
      2.0 * Math.PI * ELEVATOR_DRUM_RADIUS / 4096;
}

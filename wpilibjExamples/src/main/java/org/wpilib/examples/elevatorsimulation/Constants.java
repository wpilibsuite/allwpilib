// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorsimulation;

import org.wpilib.math.util.Units;

public class Constants {
  public static final int MOTOR_PORT = 0;
  public static final int ENCODER_A_CHANNEL = 0;
  public static final int ENCODER_B_CHANNEL = 1;
  public static final int JOYSTICK_PORT = 0;

  public static final double ELEVATOR_KP = 5;
  public static final double ELEVATOR_KI = 0;
  public static final double ELEVATOR_KD = 0;

  public static final double ELEVATOR_KS = 0.0; // volts (V)
  public static final double ELEVATOR_KG = 0.762; // volts (V)
  public static final double ELEVATOR_KV = 0.762; // volt per velocity (V/(m/s))
  public static final double ELEVATOR_KA = 0.0; // volt per acceleration (V/(m/s²))

  public static final double ELEVATOR_GEARING = 10.0;
  public static final double ELEVATOR_DRUM_RADIUS = Units.inchesToMeters(2.0);
  public static final double CARRIAGE_MASS = 4.0; // kg

  public static final double SETPOINT = 0.75; // m
  // Encoder is reset to measure 0 at the bottom, so minimum height is 0.
  public static final double MIN_ELEVATOR_HEIGHT = 0.0; // m
  public static final double MAX_ELEVATOR_HEIGHT = 1.25; // m

  // distance per pulse = (distance per revolution) / (pulses per revolution)
  //  = (Pi * D) / ppr
  public static final double ELEVATOR_ENCODER_DIST_PER_PULSE =
      2.0 * Math.PI * ELEVATOR_DRUM_RADIUS / 4096;
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.constants;

public final class ShooterConstants {
  public static final int HOOD_MOTOR_ID = 10;
  public static final int PRIMARY_SHOOTER_MOTOR_ID = 11;
  public static final int SECONDARY_SHOOTER_MOTOR_ID = 12;
  public static final int TERTIARY_SHOOTER_MOTOR_ID = 13;
  public static final int QUATERNARY_SHOOTER_MOTOR_ID = 14;

  public static final double HOOD_KP = 80;

  /** Example motor velocity feedforward constant, in volts per RPM. */
  public static final double EXAMPLE_MOTOR_KV = 12d / 6000d;

  /** Voltage necessary to break static friction to get the flywheel moving. */
  public static final double FLYWHEEL_KS = 1.25;

  public static final double FLYWHEEL_KP = 100;
}

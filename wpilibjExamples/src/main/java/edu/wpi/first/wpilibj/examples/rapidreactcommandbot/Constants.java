// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot;

import edu.wpi.first.math.util.Units;

/**
 * The Constants class provides a convenient place for teams to hold robot-wide numerical or boolean
 * constants. This class should not be used for any other purpose. All constants should be declared
 * globally (i.e. public static). Do not put anything functional in this class.
 *
 * <p>It is advised to statically import this class (or one of its inner classes) wherever the
 * constants are needed, to reduce verbosity.
 */
public final class Constants {
  public static final class DriveConstants {
    public static final int LEFT_MOTOR_1_PORT = 0;
    public static final int LEFT_MOTOR_2_PORT = 1;
    public static final int RIGHT_MOTOR_1_PORT = 2;
    public static final int RIGHT_MOTOR_2_PORT = 3;

    public static final int[] LEFT_ENCODER_PORTS = {0, 1};
    public static final int[] RIGHT_ENCODER_PORTS = {2, 3};
    public static final boolean LEFT_ENCODER_REVERSED = false;
    public static final boolean RIGHT_ENCODER_REVERSED = true;

    public static final int ENCODER_CPR = 1024;
    public static final double WHEEL_DIAMETER = Units.inchesToMeters(6);
    public static final double ENCODER_DISTANCE_PER_PULSE =
        // Assumes the encoders are directly mounted on the wheel shafts
        (WHEEL_DIAMETER * Math.PI) / ENCODER_CPR;

    // These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    // These values MUST be determined either experimentally or theoretically for *your* robot's
    // drive. The SysId tool provides a convenient method for obtaining feedback and feedforward
    // values for your robot.
    public static final double TURN_P = 1;
    public static final double TURN_I = 0;
    public static final double TURN_D = 0;

    public static final double TURN_TOLERANCE_DEG = 5;
    public static final double TURN_RATE_TOLERANCE_DEG_PER_S = 10; // degrees per second

    public static final double MAX_TURN_RATE_DEG_PER_S = 100;
    public static final double MAX_TURN_ACCELERATION_DEG_PER_S_SQUARED = 300;

    public static final double ks = 1; // V
    public static final double kv = 0.8; // V/(deg/s)
    public static final double ka = 0.15; // V/(deg/s²)
  }

  public static final class ShooterConstants {
    public static final int[] ENCODER_PORTS = {4, 5};
    public static final boolean ENCODER_REVERSED = false;
    public static final int ENCODER_CPR = 1024;
    // Distance units will be rotations
    public static final double ENCODER_DISTANCE_PER_PULSE = 1.0 / ENCODER_CPR;

    public static final int SHOOTER_MOTOR_PORT = 4;
    public static final int FEEDER_MOTOR_PORT = 5;

    public static final double SHOOTER_FREE_RPS = 5300;
    public static final double SHOOTER_TARGET_RPS = 4000;
    public static final double SHOOTER_TOLERANCE_RPS = 50;

    // These are not real PID gains, and will have to be tuned for your specific robot.
    public static final double P = 1;

    // On a real robot the feedforward constants should be empirically determined; these are
    // reasonable guesses.
    public static final double S = 0.05; // V
    // Should have value 12V at free speed
    public static final double V = 12.0 / SHOOTER_FREE_RPS; // V/(rot/s)

    public static final double FEEDER_SPEED = 0.5;
  }

  public static final class IntakeConstants {
    public static final int MOTOR_PORT = 6;
    public static final int[] SOLENOID_PORTS = {2, 3};
  }

  public static final class StorageConstants {
    public static final int MOTOR_PORT = 7;
    public static final int BALL_SENSOR_PORT = 6;
  }

  public static final class AutoConstants {
    public static final double TIMEOUT = 3;
    public static final double DRIVE_DISTANCE = 2; // m
    public static final double DRIVE_SPEED = 0.5;
  }

  public static final class OIConstants {
    public static final int DRIVER_CONTROLLER_PORT = 0;
  }
}

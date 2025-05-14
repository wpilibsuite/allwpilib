// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.drivedistanceoffboard;

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
    public static final double DT = 0.02;
    public static final int LEFT_MOTOR_1_PORT = 0;
    public static final int LEFT_MOTOR_2_PORT = 1;
    public static final int RIGHT_MOTOR_1_PORT = 2;
    public static final int RIGHT_MOTOR_2_PORT = 3;

    // These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    // These characterization values MUST be determined either experimentally or theoretically
    // for *your* robot's drive.
    // The SysId tool provides a convenient method for obtaining these values for your robot.
    public static final double S = 1; // V
    public static final double V = 0.8; // V/(m/s)
    public static final double A = 0.15; // V/(m/s²)

    public static final double P = 1;

    public static final double MAX_SPEED = 3; // m/s
    public static final double MAX_ACCELERATION = 3; // m/s²
  }

  public static final class OIConstants {
    public static final int DRIVER_CONTROLLER_PORT = 0;
  }
}

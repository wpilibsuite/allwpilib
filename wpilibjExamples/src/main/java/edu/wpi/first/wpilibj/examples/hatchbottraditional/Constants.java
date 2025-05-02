// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.hatchbottraditional;

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

    public static final int[] LEFT_ENCODER_PORTS = new int[] {0, 1};
    public static final int[] RIGHT_ENCODER_PORTS = new int[] {2, 3};
    public static final boolean LEFT_ENCODER_REVERSED = false;
    public static final boolean RIGHT_ENCODER_REVERSED = true;

    public static final int ENCODER_CPR = 1024;
    public static final double WHEEL_DIAMETER_INCHES = 6;
    public static final double ENCODER_DISTANCE_PER_PULSE =
        // Assumes the encoders are directly mounted on the wheel shafts
        (WHEEL_DIAMETER_INCHES * Math.PI) / ENCODER_CPR;
  }

  public static final class HatchConstants {
    public static final int HATCH_SOLENOID_MODULE = 0;
    public static final int[] HATCH_SOLENOID_PORTS = new int[] {0, 1};
  }

  public static final class AutoConstants {
    public static final double AUTO_DRIVE_DISTACE_INCHES = 60;
    public static final double AUTO_BACKUP_DISTANCE_INCHES = 20;
    public static final double AUTO_DRIVE_SPEED = 0.5;
  }

  public static final class OIConstants {
    public static final int DRIVER_CONTROLLER_PORT = 0;
  }
}

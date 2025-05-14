// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.mecanumcontrollercommand;

import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.trajectory.TrapezoidProfile;

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
    public static final int FRONT_LEFT_MOTOR_PORT = 0;
    public static final int REAR_LEFT_MOTOR_PORT = 1;
    public static final int FRONT_RIGHT_MOTOR_PORT = 2;
    public static final int REAR_RIGHT_MOTOR_PORT = 3;

    public static final int[] FRONT_LEFT_ENCODER_PORTS = new int[] {0, 1};
    public static final int[] REAR_LEFT_ENCODER_PORTS = new int[] {2, 3};
    public static final int[] FRONT_RIGHT_ENCODER_PORTS = new int[] {4, 5};
    public static final int[] REAR_RIGHT_ENCODER_PORTS = new int[] {6, 7};

    public static final boolean FRONT_LEFT_ENCODER_REVERSED = false;
    public static final boolean REAR_LEFT_ENCODER_REVERSED = true;
    public static final boolean FRONT_RIGHT_ENCODER_REVERSED = false;
    public static final boolean REAR_RIGHT_ENCODER_REVERSED = true;

    public static final double TRACK_WIDTH = 0.5;
    // Distance between centers of right and left wheels on robot
    public static final double WHEEL_BASE = 0.7;
    // Distance between centers of front and back wheels on robot

    public static final MecanumDriveKinematics DRIVE_KINEMATICS =
        new MecanumDriveKinematics(
            new Translation2d(WHEEL_BASE / 2, TRACK_WIDTH / 2),
            new Translation2d(WHEEL_BASE / 2, -TRACK_WIDTH / 2),
            new Translation2d(-WHEEL_BASE / 2, TRACK_WIDTH / 2),
            new Translation2d(-WHEEL_BASE / 2, -TRACK_WIDTH / 2));

    public static final int ENCODER_CPR = 1024;
    public static final double WHEEL_DIAMETER = 0.15; // m
    public static final double ENCODER_DISTANCE_PER_PULSE =
        // Assumes the encoders are directly mounted on the wheel shafts
        (WHEEL_DIAMETER * Math.PI) / ENCODER_CPR;

    // These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    // These characterization values MUST be determined either experimentally or theoretically
    // for *your* robot's drive.
    // The SysId tool provides a convenient method for obtaining these values for your robot.
    public static final SimpleMotorFeedforward FEED_FORWARD =
        new SimpleMotorFeedforward(1, 0.8, 0.15);

    // Example value only - as above, this must be tuned for your drive!
    public static final double FRONT_LEFT_VEL_P = 0.5;
    public static final double REAR_LEFT_VEL_P = 0.5;
    public static final double FRONT_RIGHT_VEL_P = 0.5;
    public static final double REAR_RIGHT_VEL_P = 0.5;
  }

  public static final class OIConstants {
    public static final int DRIVER_CONTROLLER_PORT = 0;
  }

  public static final class AutoConstants {
    public static final double MAX_SPEED = 3; // m/s
    public static final double MAX_ACCELERATION = 3; // m/s²
    public static final double MAX_ANGULAR_SPEED = Math.PI; // rad/s
    public static final double MAX_ANGULAR_ACCELERATION = Math.PI; // rad/s²

    public static final double X_CONTROLLER_P = 0.5;
    public static final double Y_CONTROLLER_P = 0.5;
    public static final double THETA_CONTROLLER_P = 0.5;

    // Constraint for the motion profilied robot angle controller
    public static final TrapezoidProfile.Constraints THETA_CONTROLLER_CONSTRAINTS =
        new TrapezoidProfile.Constraints(MAX_ANGULAR_SPEED, MAX_ANGULAR_ACCELERATION);
  }
}

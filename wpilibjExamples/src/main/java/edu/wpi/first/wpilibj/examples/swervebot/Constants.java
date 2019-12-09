/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.swervebot;

import edu.wpi.first.wpilibj.geometry.Translation2d;

/**
 * The Constants class provides a convenient place for teams to hold robot-wide
 * numerical or boolean constants. This class should not be used for any other
 * purpose. All constants should be declared globally (i.e. public static). Do
 * not put anything functional in this class.
 *
 * <p>It is advised to statically import this class (or one of its inner classes)
 * wherever the constants are needed, to reduce verbosity.
 */
public final class Constants {
  public static final class DriveConstants {
    public static final int kFrontLeftTurningMotorPort = 1;
    public static final int kFrontRightTurningMotorPort = 3;
    public static final int kBackLeftTurningMotorPort = 5;
    public static final int kBackRightTurningMotorPort = 7;

    public static final int kFrontLeftDriveMotorPort = 2;
    public static final int kFrontRightDriveMotorPort = 4;
    public static final int kBackLeftDriveMotorPort = 6;
    public static final int kBackRightDriveMotorPort = 8;

    public static final int[] kFrontLeftDriveEncoderPorts = new int[]{0, 1};
    public static final int[] kFrontRightDriveEncoderPorts = new int[]{2, 3};
    public static final int[] kBackLeftDriveEncoderPorts = new int[]{4, 5};
    public static final int[] kBackRightDriveEncoderPorts = new int[]{6, 7};

    public static final int[] kFrontLeftTurningEncoderPorts = new int[]{8, 9};
    public static final int[] kFrontRightTurningEncoderPorts = new int[]{10, 11};
    public static final int[] kBackLeftTurningEncoderPorts = new int[]{12, 13};
    public static final int[] kBackRightTurningEncoderPorts = new int[]{14, 15};

    public static final Translation2d kFrontLeftLocation = new Translation2d(0.381, 0.381);
    public static final Translation2d kFrontRightLocation = new Translation2d(0.381, -0.381);
    public static final Translation2d kBackLeftLocation = new Translation2d(-0.381, 0.381);
    public static final Translation2d kBackRightLocation = new Translation2d(-0.381, -0.381);

    public static final double kMaxSpeed = 3.0; // 3 meters per second
    public static final double kMaxAngularSpeed = Math.PI; // 1/2 rotation per second
  }

  public final class ModuleConstants {
    public static final double kWheelRadius = 0.0508;
    public static final int kEncoderResolution = 4096;

    public static final boolean kDriveEncoderReversed = false;
    public static final boolean kTurningEncoderReversed = true;

    public static final double kModuleMaxAngularVelocity = DriveConstants.kMaxAngularSpeed;
    public static final double kModuleMaxAngularAcceleration
        = 2 * Math.PI; // radians per second squared

    // The distance per pulse for the drive encoder. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    public static final double kDriveEncoderDistancePerPulse =
        2 * Math.PI * kWheelRadius / kEncoderResolution;

    // The distance (in this case, angle) per pulse for the turning encoder.
    // This is the the angle through an entire rotation (2 * wpi::math::pi)
    // divided by the encoder resolution.
    public static final double kTurningEncoderDistancePerPulse =
        2 * Math.PI / kEncoderResolution;

    public static final double kPdriveController = 1.0;
    public static final double kPturningController = 1.0;
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 1;
  }
}

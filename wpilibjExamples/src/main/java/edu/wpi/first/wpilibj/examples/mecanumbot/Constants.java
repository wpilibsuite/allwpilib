/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.mecanumbot;

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
    public static final int kFrontLeftMotorPort = 0;
    public static final int kFrontRightMotorPort = 1;
    public static final int kBackLeftMotorPort = 2;
    public static final int kBackRightMotorPort = 3;

    public static final int[] kFrontLeftEncoderPorts = new int[]{0, 1};
    public static final int[] kFrontRightEncoderPorts = new int[]{2, 3};
    public static final int[] kBackLeftEncoderPorts = new int[]{4, 5};
    public static final int[] kBackRightEncoderPorts = new int[]{6, 7};

    public static final boolean kFrontLeftEncoderReversed = false;
    public static final boolean kFrontRightEncoderReversed = true;
    public static final boolean kBackLeftEncoderReversed = false;
    public static final boolean kBackRightEncoderReversed = true;

    public static final Translation2d kFrontLeftLocation = new Translation2d(0.381, 0.381);
    public static final Translation2d kFrontRightLocation = new Translation2d(0.381, -0.381);
    public static final Translation2d kBackLeftLocation = new Translation2d(-0.381, 0.381);
    public static final Translation2d kBackRightLocation = new Translation2d(-0.381, -0.381);

    public static final double kPfrontLeftController = 1.0;
    public static final double kPfrontRightController = 1.0;
    public static final double kPbackLeftController = 1.0;
    public static final double kPbackRightController = 1.0;

    public static final double kMaxSpeed = 3.0; // 3 meters per second
    public static final double kMaxAngularSpeed = Math.PI; // 1/2 rotation per second
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 1;
  }
}

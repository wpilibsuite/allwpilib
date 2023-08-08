// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.differentialdrivebot;

public class Constants {
  public static final class DriveConstants {
    public static final int kLeftMotor1Port = 1;
    public static final int kLeftMotor2Port = 2;
    public static final int kRightMotor1Port = 3;
    public static final int kRightMotor2Port = 4;

    public static final int kLeftEncoderPorts[] = new int[] {0, 1};
    public static final int kRightEncoderPorts[] = new int[] {2, 3};

    public static final int kGyroPort = 0;

    public static final double kMaxSpeed = 3.0; // meters per second
    public static final double kMaxAngularSpeed = 2 * Math.PI; // one rotation per second

    public static final double kTrackWidth = 0.381 * 2; // meters
    public static final double kWheelRadius = 0.0508; // meters
    public static final int kEncoderResolution = 4096;

    public static final double kLeftVelP = 1;
    public static final double kRightVelP = 1;

    public static final double kFeedforwardS = 1;
    public static final double kFeedforwardV = 3;

    public static final double kVelSlewRate = 3;
    public static final double kRotSlewRate = 3;
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 0;
  }
}

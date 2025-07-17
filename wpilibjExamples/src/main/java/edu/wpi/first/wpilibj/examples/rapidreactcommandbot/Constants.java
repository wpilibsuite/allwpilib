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
    public static final int kLeftMotor1Port = 0;
    public static final int kLeftMotor2Port = 1;
    public static final int kRightMotor1Port = 2;
    public static final int kRightMotor2Port = 3;

    public static final int[] kLeftEncoderPorts = {0, 1};
    public static final int[] kRightEncoderPorts = {2, 3};
    public static final boolean kLeftEncoderReversed = false;
    public static final boolean kRightEncoderReversed = true;

    public static final int kEncoderCPR = 1024;
    public static final double kWheelDiameter = Units.inchesToMeters(6);
    public static final double kEncoderDistancePerPulse =
        // Assumes the encoders are directly mounted on the wheel shafts
        (kWheelDiameter * Math.PI) / kEncoderCPR;

    // These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    // These values MUST be determined either experimentally or theoretically for *your* robot's
    // drive. The SysId tool provides a convenient method for obtaining feedback and feedforward
    // values for your robot.
    public static final double kTurnP = 1;
    public static final double kTurnI = 0;
    public static final double kTurnD = 0;

    public static final double kTurnToleranceDeg = 5;
    public static final double kTurnRateToleranceDegPerS = 10; // degrees per second

    public static final double kMaxTurnRateDegPerS = 100;
    public static final double kMaxTurnAccelerationDegPerSSquared = 300;

    public static final double ks = 1; // V
    public static final double kv = 0.8; // V/(deg/s)
    public static final double ka = 0.15; // V/(deg/s²)
  }

  public static final class ShooterConstants {
    public static final int[] kEncoderPorts = {4, 5};
    public static final boolean kEncoderReversed = false;
    public static final int kEncoderCPR = 1024;
    // Distance units will be rotations
    public static final double kEncoderDistancePerPulse = 1.0 / kEncoderCPR;

    public static final int kShooterMotorPort = 4;
    public static final int kFeederMotorPort = 5;

    public static final double kShooterFreeRPS = 5300;
    public static final double kShooterTargetRPS = 4000;
    public static final double kShooterToleranceRPS = 50;

    // These are not real PID gains, and will have to be tuned for your specific robot.
    public static final double kP = 1;

    // On a real robot the feedforward constants should be empirically determined; these are
    // reasonable guesses.
    public static final double kS = 0.05; // V
    // Should have value 12V at free speed
    public static final double kV = 12.0 / kShooterFreeRPS; // V/(rot/s)

    public static final double kFeederSpeed = 0.5;
  }

  public static final class IntakeConstants {
    public static final int kMotorPort = 6;
    public static final int[] kSolenoidPorts = {2, 3};
  }

  public static final class StorageConstants {
    public static final int kMotorPort = 7;
    public static final int kBallSensorPort = 6;
  }

  public static final class AutoConstants {
    public static final double kTimeout = 3;
    public static final double kDriveDistance = 2; // m
    public static final double kDriveSpeed = 0.5;
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 0;
  }
}

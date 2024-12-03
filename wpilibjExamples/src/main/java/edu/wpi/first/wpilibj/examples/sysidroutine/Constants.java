// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.sysidroutine;

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
    public static final double kWheelDiameterMeters = Units.inchesToMeters(6);
    public static final double kEncoderDistancePerPulse =
        // Assumes the encoders are directly mounted on the wheel shafts
        (kWheelDiameterMeters * Math.PI) / kEncoderCPR;
  }

  public static final class ShooterConstants {
    public static final int[] kEncoderPorts = {4, 5};
    public static final boolean kEncoderReversed = false;
    public static final int kEncoderCPR = 1024;
    public static final double kEncoderDistancePerPulse =
        // Distance units will be rotations
        1.0 / kEncoderCPR;

    public static final int kShooterMotorPort = 4;
    public static final int kFeederMotorPort = 5;

    public static final double kShooterFreeRPS = 5300;
    public static final double kShooterTargetRPS = 4000;
    public static final double kShooterToleranceRPS = 50;

    // These are not real PID gains, and will have to be tuned for your specific robot.
    public static final double kP = 1;

    // On a real robot the feedforward constants should be empirically determined; these are
    // reasonable guesses.
    public static final double kSVolts = 0.05;
    public static final double kVVoltSecondsPerRotation =
        // Should have value 12V at free speed...
        12.0 / kShooterFreeRPS;
    public static final double kAVoltSecondsSquaredPerRotation = 0;

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
    public static final double kTimeoutSeconds = 3;
    public static final double kDriveDistanceMeters = 2;
    public static final double kDriveSpeed = 0.5;
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 0;
  }
}

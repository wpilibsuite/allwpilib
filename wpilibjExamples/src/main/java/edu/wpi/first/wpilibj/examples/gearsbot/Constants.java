// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot;

public final class Constants {
  public static final class DriveConstants {
    public static final int kLeftMotorPort1 = 0;
    public static final int kLeftMotorPort2 = 1;

    public static final int kRightMotorPort1 = 2;
    public static final int kRightMotorPort2 = 3;

    public static final int[] kLeftEncoderPorts = {0, 1};
    public static final int[] kRightEncoderPorts = {2, 3};
    public static final boolean kLeftEncoderReversed = false;
    public static final boolean kRightEncoderReversed = false;

    public static final int kRangeFinderPort = 6;
    public static final int kAnalogGyroPort = 1;

    public static final int kEncoderCPR = 1024;
    public static final double kWheelDiameterInches = 6;
    public static final double kEncoderDistancePerPulse =
        // Assumes the encoders are directly mounted on the wheel shafts
        (kWheelDiameterInches * Math.PI) / (double) kEncoderCPR;
  }

  public static final class ClawConstants {
    public static final int kMotorPort = 7;
    public static final int kContactPort = 5;
  }

  public static final class WristConstants {
    public static final int kMotorPort = 6;

    // these pid constants are not real, and will need to be tuned
    public static final double kP = 0.1;
    public static final double kI = 0.0;
    public static final double kD = 0.0;

    public static final double kTolerance = 2.5;

    public static final int kPotentiometerPort = 3;
  }

  public static final class ElevatorConstants {
    public static final int kMotorPort = 5;
    public static final int kPotentiometerPort = 2;

    // these pid constants are not real, and will need to be tuned
    public static final double kP_real = 4;
    public static final double kI_real = 0.007;

    public static final double kP_simulation = 18;
    public static final double kI_simulation = 0.2;

    public static final double kD = 0.0;

    public static final double kTolerance = 0.005;
  }

  public static final class AutoConstants {
    public static final double kDistToBox1 = 0.10;
    public static final double kDistToBox2 = 0.60;

    public static final double kWristSetpoint = -45.0;
  }

  public static final class DriveStraightConstants {
    // these pid constants are not real, and will need to be tuned
    public static final double kP = 4.0;
    public static final double kI = 0.0;
    public static final double kD = 0.0;
  }

  public static final class Positions {
    public static final class Pickup {
      public static final double kWristSetpoint = -45.0;
      public static final double kElevatorSetpoint = 0.25;
    }

    public static final class Place {
      public static final double kWristSetpoint = 0.0;
      public static final double kElevatorSetpoint = 0.25;
    }

    public static final class PrepareToPickup {
      public static final double kWristSetpoint = 0.0;
      public static final double kElevatorSetpoint = 0.0;
    }
  }
}

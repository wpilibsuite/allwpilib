/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat;

import edu.wpi.first.wpilibj.CounterBase.EncodingType;

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
    public static final int kFrontLeftCIMPort = 0;
    public static final int kFrontRightCIMPort = 1;
    public static final int kRearLeftCIMPort = 2;
    public static final int kRearRightCIMPort = 3;

    public static final int[] kLeftEncoderPorts = new int[]{1, 2};
    public static final int[] kRightEncoderPorts = new int[]{3, 4};

    public static final boolean kLeftEncoderReversed = false;
    public static final boolean kRightEncoderReversed = true;

    public static final EncodingType kLeftEncoderEncodingType = EncodingType.k4X;
    public static final EncodingType kRightEncoderEncodingType = EncodingType.k4X;

    // Converts to feet
    public static final double kRealEncoderDistancePerPulse = 0.0785398;

    // Convert to feet 4in diameter wheels with 360 tick sim encoders
    public static final int kSimEncoderResolution = 360;
    public static final double kSimWheelRadius = 2; // inches
    public static final double kSimEncoderDistancePerPulse =
        // Assumes the encoders are directly mounted on the wheel shafts
        (2 * Math.PI * kSimWheelRadius/* in */) / (kSimEncoderResolution * 12.0/* in/ft */);
  }

  public final class CollectorConstants {
    public static final int kRollerMotorPort = 6;
    public static final int kBallDetectorPort = 10;
    public static final int kOpenDetectorPort = 6;

    public static final int kPistonModuleNumber = 1;
    public static final int kPistonChannel = 1;
  }

  public final class PivotConstants {
    public static final int kUpperLimitSwitchPort = 13;
    public static final int kLowerLimitSwitchPort = 12;

    public static final int kPivotPotPort = 1;

    public static final int kPivotMotorPort = 5;

    public static final double kPRealPivotController = 7.0;
    public static final double kIRealPivotController = 0.0;
    public static final double kDRealPivotController = 8.0;
    public static final double kToleranceRealPivotController = 0.005;

    public static final double kPSimPivotController = 0.5;
    public static final double kISimPivotController = 0.001;
    public static final double kDSimPivotController = 0.2;
    public static final double kToleranceSimPivotController = 5;
  }

  public final class PneumaticsConstants {
    public static final int kPressureSensorPort = 3;
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 0;
  }
}

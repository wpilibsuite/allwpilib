/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot;


/**
 * The Constants class provides a convenient place for teams to hold robot-wide numerical or boolean
 * constants.  This class should not be used for any other purpose.  All constants should be
 * declared globally (i.e. public static).  Do not put anything functional in this class.
 *
 * <p>It is advised to statically import this class (or one of its inner classes) wherever the
 * constants are needed, to reduce verbosity.
 */
public final class Constants {
  public static final class DriveConstants {
    public static final int kLeftFrontPort = 0;
    public static final int kLeftRearPort = 1;
    public static final int kRightFrontPort = 2;
    public static final int kRightRearPort = 3;

    public static final int[] kLeftEncoderPorts = new int[]{0, 1};
    public static final int[] kRightEncoderPorts = new int[]{2, 3};

    public static final boolean kLeftEncoderReversed = false;
    public static final boolean kRightEncoderReversed = true;

    public static final int kRangeFinderPort = 6;
    
    // We have to convert the wheel diameter to feet then use the circumference formula
    // Circumference in ft = 4in / 12(in/ft) * PI
    public static final int kSimEncoderResolution = 360;
    public static final double kSimWheelRadius = 2; // inches
    public static final double kSimEncoderDistancePerPulse =
        // Assumes the encoders are directly mounted on the wheel shafts
        ((2 * kSimWheelRadius) / 12) * Math.PI / kSimEncoderResolution;

    public static final double kRealEncoderDistancePerPulse = 0.042; // barleycorns

    public static final double kPDrive = -2;
  }

  public static final class ClawConstants {
    public static final int kClawPort = 7;
    public static final int kContactPort = 5;
  }

  public static final class ElevatorConstants {
    public static final int kElevatorPort = 5;
    public static final int kPotPort = 2;

    public static final double kP_real = 4;
    public static final double kI_real = 0.07;
    public static final double kP_simulation = 18;
    public static final double kI_simulation = 0.2;
  }

  public static final class WristConstants {
    public static final int kWristPort = 6;
    public static final int kPotPort = 3;

    public static final double kP_real = 1;
    public static final double kP_simulation = 0.05;
  }

  public static final class OIConstants {
    public static final int kDriverControllerPort = 0;
  }

}

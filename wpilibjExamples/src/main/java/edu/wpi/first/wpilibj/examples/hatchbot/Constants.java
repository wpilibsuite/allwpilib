package edu.wpi.first.wpilibj.examples.hatchbot;

/**
 * The Constants class provides a convenient place for teams to hold robot-wide numerical or boolean
 * constants.  This class should not be used for any other purpose.  All constants should be
 * declared globally (i.e. public static).  Do not put anything functional in this class.
 */
public class Constants {

  // Drive Constants
  public static final int kLeftMotor1Port = 0;
  public static final int kLeftMotor2Port = 1;
  public static final int kRightMotor1Port = 2;
  public static final int kRightMotor2Port = 3;

  public static final int[] kLeftEncoderPorts = new int[]{0, 1};
  public static final int[] kRightEncoderPorts = new int[]{2, 3};

  public static final int kEncoderCPR = 1024;
  public static final double kWheelDiameterInches = 6;
  public static final double kEncoderDistancePerPulse =
      // Assumes the encoders are directly mounted on the wheel shafts
      (kWheelDiameterInches * Math.PI) / (double) kEncoderCPR;

  // Hatch Constants
  public static final int kHatchSolenoidModule = 0;
  public static final int[] kHatchSolenoidPorts = new int[]{0, 1};

  // Autonomous Constants
  public static final double kAutoDriveDistanceInches = 60;
  public static final double kAutoBackupDistanceInches = 20;

  // OI Constants
  public static final int kDriverControllerPort = 1;
}

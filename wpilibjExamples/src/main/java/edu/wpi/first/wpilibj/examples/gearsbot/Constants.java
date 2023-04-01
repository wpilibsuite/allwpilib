package edu.wpi.first.wpilibj.examples.gearsbot;


public  final class Constants {
  
  public static final class DriveConstants{
    public static final int leftMotorPort1 = 0;
    public static final int leftMotorPort2 = 1;

    public static final int rightMotorPort1 = 2;
    public static final int rightMotorPort2 = 3;


    public static final int[] leftEncoderPorts = {0,1};
    public static final int[] rightEncoderPorts = {2,3};
    public static final boolean leftEncoderReversed = false;
    public static final boolean rightEncoderReversed = false;

    public static final int rangeFinderPort = 6;
    public static final int analogGyroPort = 1;

    public static final int encoderCPR = 1024;
    public static final double wheelDiameterInches = 6;
    public static final double encoderDistancePerPulse = 
      // Assumes the encoders are directly mounted on the wheel shafts
      (wheelDiameterInches * Math.PI) / (double) encoderCPR;

  }

  public static final class ClawConstants{
    public static final int motorPort = 7;
    public static final int contactPort = 5; 
  }

  public static final class WristConstants{
    public static final int motorPort = 6;

    //these pid constants are not real, and will need to be tuned
    public static final double kP = 0.1;
    public static final double kI = 0.0;
    public static final double kD = 0.0;

    public static final double kTolerance = 2.5;

    public static final int potentiometerPort = 3;
  };
  
  public static final class ElevatorConstants{
    public static final int motorPort = 5;
    public static final int potentiometerPort = 2;
    
    //these pid constants are not real, and will need to be tuned
    public static final double kP_real = 4;
    public static final double kI_real = 0.007;
    
    public static final double kP_simulation = 18;
    public static final double kI_simulation = 0.2;

    public static final double kD = 0.0;

    public static final double kTolerance = 0.005;
  };

  public static final class AutoConstants{
    public static final double distToBox1 = 0.10;
    public static final double distToBox2 = 0.60;
  }
}

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
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.drive;

/** Common base class for drive platforms. */
public class RobotDriveBase {
  public static final double kDefaultMaxOutput = 1.0;

  protected double m_maxOutput = kDefaultMaxOutput;

  /** The location of a motor on the robot for the purpose of driving. */
  public enum MotorType {
    kFrontLeft(0),
    kFrontRight(1),
    kRearLeft(2),
    kRearRight(3),
    kLeft(0),
    kRight(1),
    kBack(2);

    @SuppressWarnings("MemberName")
    public final int value;

    MotorType(int value) {
      this.value = value;
    }
  }

  /** RobotDriveBase constructor. */
  protected RobotDriveBase() {}

  /**
   * Configure the scaling factor for using drive methods with motor controllers in a mode other
   * than PercentVbus or to limit the maximum output.
   *
   * <p>The default value is {@value #kDefaultMaxOutput}.
   *
   * @param maxOutput Multiplied with the output percentage computed by the drive functions.
   */
  public void setMaxOutput(double maxOutput) {
    m_maxOutput = maxOutput;
  }

  /**
   * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
   *
   * @param wheelSpeeds Array of wheel speeds.
   */
  protected static void normalize(double[] wheelSpeeds) {
    double maxMagnitude = Math.abs(wheelSpeeds[0]);
    for (int i = 1; i < wheelSpeeds.length; i++) {
      double temp = Math.abs(wheelSpeeds[i]);
      if (maxMagnitude < temp) {
        maxMagnitude = temp;
      }
    }
    if (maxMagnitude > 1.0) {
      for (int i = 0; i < wheelSpeeds.length; i++) {
        wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
      }
    }
  }
}

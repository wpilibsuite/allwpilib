// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drive;

import org.wpilib.hardware.motor.MotorSafety;

/**
 * Common base class for drive platforms.
 *
 * <p>{@link org.wpilib.hardware.motor.MotorSafety} is enabled by default.
 */
public abstract class RobotDriveBase extends MotorSafety {
  /** Default input deadband. */
  public static final double kDefaultDeadband = 0.02;

  /** Default maximum output. */
  public static final double kDefaultMaxOutput = 1.0;

  /** Input deadband. */
  protected double m_deadband = kDefaultDeadband;

  /** Maximum output. */
  protected double m_maxOutput = kDefaultMaxOutput;

  /** The location of a motor on the robot for the purpose of driving. */
  public enum MotorType {
    /** Front left motor. */
    kFrontLeft(0),
    /** Front right motor. */
    kFrontRight(1),
    /** Rear left motor. */
    kRearLeft(2),
    /** Rear right motor. */
    kRearRight(3),
    /** Left motor. */
    kLeft(0),
    /** Right motor. */
    kRight(1),
    /** Back motor. */
    kBack(2);

    /** MotorType value. */
    public final int value;

    MotorType(int value) {
      this.value = value;
    }
  }

  /** RobotDriveBase constructor. */
  @SuppressWarnings("this-escape")
  public RobotDriveBase() {
    setSafetyEnabled(true);
  }

  /**
   * Sets the deadband applied to the drive inputs (e.g., joystick values).
   *
   * <p>The default value is {@value #kDefaultDeadband}. Inputs smaller than the deadband are set to
   * 0.0 while inputs larger than the deadband are scaled from 0.0 to 1.0. See {@link
   * org.wpilib.math.util.MathUtil#applyDeadband}.
   *
   * @param deadband The deadband to set.
   */
  public void setDeadband(double deadband) {
    m_deadband = deadband;
  }

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
   * Feed the motor safety object. Resets the timer that will stop the motors if it completes.
   *
   * @see MotorSafety#feed()
   */
  public void feedWatchdog() {
    feed();
  }

  @Override
  public abstract void stopMotor();

  @Override
  public abstract String getDescription();

  /**
   * Normalize all wheel velocities if the magnitude of any wheel is greater than 1.0.
   *
   * @param wheelVelocities List of wheel velocities to normalize.
   */
  protected static void normalize(double[] wheelVelocities) {
    double maxMagnitude = Math.abs(wheelVelocities[0]);
    for (int i = 1; i < wheelVelocities.length; i++) {
      double temp = Math.abs(wheelVelocities[i]);
      if (maxMagnitude < temp) {
        maxMagnitude = temp;
      }
    }
    if (maxMagnitude > 1.0) {
      for (int i = 0; i < wheelVelocities.length; i++) {
        wheelVelocities[i] = wheelVelocities[i] / maxMagnitude;
      }
    }
  }
}

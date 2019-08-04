/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.drive;

import edu.wpi.first.wpilibj.MotorSafety;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.SendableImpl;

/**
 * Common base class for drive platforms.
 */
public abstract class RobotDriveBase extends MotorSafety implements Sendable, AutoCloseable {
  public static final double kDefaultDeadband = 0.02;
  public static final double kDefaultMaxOutput = 1.0;

  protected double m_deadband = kDefaultDeadband;
  protected double m_maxOutput = kDefaultMaxOutput;

  private final SendableImpl m_sendableImpl;

  /**
   * The location of a motor on the robot for the purpose of driving.
   */
  public enum MotorType {
    kFrontLeft(0), kFrontRight(1), kRearLeft(2), kRearRight(3), kLeft(0),
    kRight(1), kBack(2);

    @SuppressWarnings("MemberName")
    public final int value;

    MotorType(int value) {
      this.value = value;
    }
  }

  /**
   * RobotDriveBase constructor.
   */
  public RobotDriveBase() {
    m_sendableImpl = new SendableImpl(true);

    setSafetyEnabled(true);
    setName("RobotDriveBase");
  }

  @Override
  public void close() {
    m_sendableImpl.close();
  }

  @Override
  public final synchronized String getName() {
    return m_sendableImpl.getName();
  }

  @Override
  public final synchronized void setName(String name) {
    m_sendableImpl.setName(name);
  }

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel    The channel number the device is plugged into
   */
  protected final void setName(String moduleType, int channel) {
    m_sendableImpl.setName(moduleType, channel);
  }

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @param moduleType   A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually PWM)
   */
  protected final void setName(String moduleType, int moduleNumber, int channel) {
    m_sendableImpl.setName(moduleType, moduleNumber, channel);
  }

  @Override
  public final synchronized String getSubsystem() {
    return m_sendableImpl.getSubsystem();
  }

  @Override
  public final synchronized void setSubsystem(String subsystem) {
    m_sendableImpl.setSubsystem(subsystem);
  }

  /**
   * Add a child component.
   *
   * @param child child component
   */
  protected final void addChild(Object child) {
    m_sendableImpl.addChild(child);
  }

  /**
   * Sets the deadband applied to the drive inputs (e.g., joystick values).
   *
   * <p>The default value is {@value #kDefaultDeadband}. Inputs smaller than the deadband are set to
   * 0.0 while inputs larger than the deadband are scaled from 0.0 to 1.0. See
   * {@link #applyDeadband}.
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
   * Limit motor values to the -1.0 to +1.0 range.
   */
  protected double limit(double value) {
    if (value > 1.0) {
      return 1.0;
    }
    if (value < -1.0) {
      return -1.0;
    }
    return value;
  }

  /**
   * Returns 0.0 if the given value is within the specified range around zero. The remaining range
   * between the deadband and 1.0 is scaled from 0.0 to 1.0.
   *
   * @param value    value to clip
   * @param deadband range around zero
   */
  protected double applyDeadband(double value, double deadband) {
    if (Math.abs(value) > deadband) {
      if (value > 0.0) {
        return (value - deadband) / (1.0 - deadband);
      } else {
        return (value + deadband) / (1.0 - deadband);
      }
    } else {
      return 0.0;
    }
  }

  /**
   * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
   */
  protected void normalize(double[] wheelSpeeds) {
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

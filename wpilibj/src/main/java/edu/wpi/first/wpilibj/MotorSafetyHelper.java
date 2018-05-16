/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.LinkedHashSet;
import java.util.Set;

/**
 * The MotorSafetyHelper object is constructed for every object that wants to implement the Motor
 * Safety protocol. The helper object has the code to actually do the timing and call the motors
 * Stop() method when the timeout expires. The motor object is expected to call the Feed() method
 * whenever the motors value is updated.
 */
public final class MotorSafetyHelper {
  private double m_expiration;
  private boolean m_enabled;
  private double m_stopTime;
  private final Object m_thisMutex = new Object();
  private final MotorSafety m_safeObject;
  private static final Set<MotorSafetyHelper> m_helperList = new LinkedHashSet<>();
  private static final Object m_listMutex = new Object();

  /**
   * The constructor for a MotorSafetyHelper object. The helper object is constructed for every
   * object that wants to implement the Motor Safety protocol. The helper object has the code to
   * actually do the timing and call the motors Stop() method when the timeout expires. The motor
   * object is expected to call the Feed() method whenever the motors value is updated.
   *
   * @param safeObject a pointer to the motor object implementing MotorSafety. This is used to call
   *                   the Stop() method on the motor.
   */
  public MotorSafetyHelper(MotorSafety safeObject) {
    m_safeObject = safeObject;
    m_enabled = false;
    m_expiration = MotorSafety.DEFAULT_SAFETY_EXPIRATION;
    m_stopTime = Timer.getFPGATimestamp();

    synchronized (m_listMutex) {
      m_helperList.add(this);
    }
  }

  /**
   * Feed the motor safety object. Resets the timer on this object that is used to do the timeouts.
   */
  public void feed() {
    synchronized (m_thisMutex) {
      m_stopTime = Timer.getFPGATimestamp() + m_expiration;
    }
  }

  /**
   * Set the expiration time for the corresponding motor safety object.
   *
   * @param expirationTime The timeout value in seconds.
   */
  public void setExpiration(double expirationTime) {
    synchronized (m_thisMutex) {
      m_expiration = expirationTime;
    }
  }

  /**
   * Retrieve the timeout value for the corresponding motor safety object.
   *
   * @return the timeout value in seconds.
   */
  public double getExpiration() {
    synchronized (m_thisMutex) {
      return m_expiration;
    }
  }

  /**
   * Determine of the motor is still operating or has timed out.
   *
   * @return a true value if the motor is still operating normally and hasn't timed out.
   */
  public boolean isAlive() {
    synchronized (m_thisMutex) {
      return !m_enabled || m_stopTime > Timer.getFPGATimestamp();
    }
  }

  /**
   * Check if this motor has exceeded its timeout. This method is called periodically to determine
   * if this motor has exceeded its timeout value. If it has, the stop method is called, and the
   * motor is shut down until its value is updated again.
   */
  public void check() {
    boolean enabled;
    double stopTime;

    synchronized (m_thisMutex) {
      enabled = m_enabled;
      stopTime = m_stopTime;
    }

    if (!enabled || RobotState.isDisabled() || RobotState.isTest()) {
      return;
    }
    if (stopTime < Timer.getFPGATimestamp()) {
      DriverStation.reportError(m_safeObject.getDescription() + "... Output not updated often "
          + "enough.", false);

      m_safeObject.stopMotor();
    }
  }

  /**
   * Enable/disable motor safety for this device Turn on and off the motor safety option for this
   * PWM object.
   *
   * @param enabled True if motor safety is enforced for this object
   */
  public void setSafetyEnabled(boolean enabled) {
    synchronized (m_thisMutex) {
      m_enabled = enabled;
    }
  }

  /**
   * Return the state of the motor safety enabled flag Return if the motor safety is currently
   * enabled for this device.
   *
   * @return True if motor safety is enforced for this device
   */
  public boolean isSafetyEnabled() {
    synchronized (m_thisMutex) {
      return m_enabled;
    }
  }

  /**
   * Check the motors to see if any have timed out. This static method is called periodically to
   * poll all the motors and stop any that have timed out.
   */
  public static void checkMotors() {
    synchronized (m_listMutex) {
      for (MotorSafetyHelper elem : m_helperList) {
        elem.check();
      }
    }
  }
}

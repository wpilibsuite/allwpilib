/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * This base class runs a watchdog timer and calls the subclass's StopMotor()
 * function if the timeout expires.
 *
 * <p>The subclass should call feed() whenever the motor value is updated.
 */
public abstract class MotorSafety {
  private static final double kDefaultSafetyExpiration = 0.1;

  private final Watchdog m_watchdog = new Watchdog(kDefaultSafetyExpiration, this::timeoutFunc);
  private boolean m_enabled;
  private final Object m_thisMutex = new Object();

  public MotorSafety() {
    m_watchdog.suppressTimeoutMessage(true);
  }

  /**
   * Feed the motor safety object.
   *
   * <p>Resets the timer on this object that is used to do the timeouts.
   */
  public void feed() {
    synchronized (m_thisMutex) {
      m_watchdog.reset();
    }
  }

  /**
   * Set the expiration time for the corresponding motor safety object.
   *
   * @param expirationTime The timeout value in seconds.
   */
  public void setExpiration(double expirationTime) {
    synchronized (m_thisMutex) {
      m_watchdog.setTimeout(expirationTime);
    }
  }

  /**
   * Retrieve the timeout value for the corresponding motor safety object.
   *
   * @return the timeout value in seconds.
   */
  public double getExpiration() {
    synchronized (m_thisMutex) {
      return m_watchdog.getTimeout();
    }
  }

  /**
   * Determine of the motor is still operating or has timed out.
   *
   * @return a true value if the motor is still operating normally and hasn't timed out.
   */
  public boolean isAlive() {
    synchronized (m_thisMutex) {
      return !m_enabled || !m_watchdog.isExpired();
    }
  }

  /**
   * Enable/disable motor safety for this device.
   *
   * <p>Turn on and off the motor safety option for this PWM object.
   *
   * @param enabled True if motor safety is enforced for this object
   */
  public void setSafetyEnabled(boolean enabled) {
    synchronized (m_thisMutex) {
      if (enabled) {
        m_watchdog.enable();
      } else {
        m_watchdog.disable();
      }
      m_enabled = enabled;
    }
  }

  /**
   * Return the state of the motor safety enabled flag.
   *
   * <p>Return if the motor safety is currently enabled for this device.
   *
   * @return True if motor safety is enforced for this device
   */
  public boolean isSafetyEnabled() {
    synchronized (m_thisMutex) {
      return m_enabled;
    }
  }

  private void timeoutFunc() {
    DriverStation ds = DriverStation.getInstance();
    if (ds.isDisabled() || ds.isTest()) {
      return;
    }

    DriverStation.reportError(getDescription() + "... Output not updated often enough.", false);

    stopMotor();
  }

  public abstract void stopMotor();

  public abstract String getDescription();
}

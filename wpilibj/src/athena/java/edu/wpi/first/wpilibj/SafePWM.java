/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 *
 * @author brad
 */
public class SafePWM extends PWM implements MotorSafety {

  private MotorSafetyHelper m_safetyHelper;

  /**
   * Initialize a SafePWM object by setting defaults
   */
  void initSafePWM() {
    m_safetyHelper = new MotorSafetyHelper(this);
    m_safetyHelper.setExpiration(0.0);
    m_safetyHelper.setSafetyEnabled(false);
  }


  /**
   * Constructor for a SafePWM object taking a channel number
   *$
   * @param channel The channel number to be used for the underlying PWM object.
   *        0-9 are on-board, 10-19 are on the MXP port.
   */
  public SafePWM(final int channel) {
    super(channel);
    initSafePWM();
  }

  /*
   * Set the expiration time for the PWM object
   *$
   * @param timeout The timeout (in seconds) for this motor object
   */
  public void setExpiration(double timeout) {
    m_safetyHelper.setExpiration(timeout);
  }

  /**
   * Return the expiration time for the PWM object.
   *$
   * @return The expiration time value.
   */
  public double getExpiration() {
    return m_safetyHelper.getExpiration();
  }

  /**
   * Check if the PWM object is currently alive or stopped due to a timeout.
   *$
   * @return a bool value that is true if the motor has NOT timed out and should
   *         still be running.
   */
  public boolean isAlive() {
    return m_safetyHelper.isAlive();
  }

  /**
   * Stop the motor associated with this PWM object. This is called by the
   * MotorSafetyHelper object when it has a timeout for this PWM and needs to
   * stop it from running.
   */
  public void stopMotor() {
    disable();
  }

  /**
   * Check if motor safety is enabled for this object
   *$
   * @return True if motor safety is enforced for this object
   */
  public boolean isSafetyEnabled() {
    return m_safetyHelper.isSafetyEnabled();
  }

  /**
   * Feed the MotorSafety timer. This method is called by the subclass motor
   * whenever it updates its speed, thereby reseting the timeout value.
   */
  public void Feed() {
    m_safetyHelper.feed();
  }

  public void setSafetyEnabled(boolean enabled) {
    m_safetyHelper.setSafetyEnabled(enabled);
  }

  public String getDescription() {
    return "PWM " + getChannel();
  }

  public void disable() {
    setRaw(kPwmDisabled);
  }
}

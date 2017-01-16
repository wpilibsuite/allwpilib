/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Common base class for all PWM Speed Controllers.
 */
public abstract class PWMSpeedController extends SafePWM implements SpeedController {
  private boolean m_isInverted = false;

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the controller is attached to. 0-9 are on-board, 10-19 are
   *                on the MXP port
   */
  protected PWMSpeedController(int channel) {
    super(channel);
  }

  /**
   * Set the PWM value.
   *
   * <p>The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  @Override
  public void set(double speed) {
    setSpeed(m_isInverted ? -speed : speed);
    feed();
  }

  /**
   * Common interface for inverting direction of a speed controller.
   *
   * @param isInverted The state of inversion true is inverted
   */
  @Override
  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  /**
   * Common interface for the inverting direction of a speed controller.
   *
   * @return isInverted The state of inversion, true is inverted.
   */
  @Override
  public boolean getInverted() {
    return m_isInverted;
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return getSpeed();
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  @Override
  public void pidWrite(double output) {
    set(output);
  }
}

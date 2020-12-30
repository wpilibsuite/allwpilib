// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/** Common base class for all PWM Speed Controllers. */
public abstract class PWMSpeedController extends PWM implements SpeedController {
  private boolean m_isInverted;

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the controller is attached to. 0-9 are on-board, 10-19 are
   *     on the MXP port
   */
  protected PWMSpeedController(int channel) {
    super(channel);
  }

  @Override
  public String getDescription() {
    return "PWM " + getChannel();
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
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return getSpeed() * (m_isInverted ? -1.0 : 1.0);
  }

  @Override
  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  @Override
  public boolean getInverted() {
    return m_isInverted;
  }

  @Override
  public void disable() {
    setDisabled();
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

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Speed Controller");
    builder.setActuator(true);
    builder.setSafeState(this::setDisabled);
    builder.addDoubleProperty("Value", this::getSpeed, this::setSpeed);
  }
}

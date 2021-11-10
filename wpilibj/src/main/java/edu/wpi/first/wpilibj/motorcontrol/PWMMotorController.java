// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.MotorSafety;
import edu.wpi.first.wpilibj.PWM;

/** Common base class for all PWM Motor Controllers. */
public abstract class PWMMotorController extends MotorSafety
    implements MotorController, Sendable, AutoCloseable {
  private boolean m_isInverted;
  protected PWM m_pwm;

  /**
   * Constructor.
   *
   * @param name Name to use for SendableRegistry
   * @param channel The PWM channel that the controller is attached to. 0-9 are on-board, 10-19 are
   *     on the MXP port
   */
  protected PWMMotorController(final String name, final int channel) {
    m_pwm = new PWM(channel, false);
    SendableRegistry.addLW(this, name, channel);
  }

  /** Free the resource associated with the PWM channel and set the value to 0. */
  @Override
  public void close() {
    SendableRegistry.remove(this);
    m_pwm.close();
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
    m_pwm.setSpeed(m_isInverted ? -speed : speed);
    feed();
  }

  /**
   * Get the recently set value of the PWM. This value is affected by the inversion property. If you
   * want the value that is sent directly to the MotorController, use {@link
   * edu.wpi.first.wpilibj.PWM#getSpeed()} instead.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return m_pwm.getSpeed() * (m_isInverted ? -1.0 : 1.0);
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
    m_pwm.setDisabled();
  }

  @Override
  public void stopMotor() {
    disable();
  }

  @Override
  public String getDescription() {
    return "PWM " + getChannel();
  }

  /**
   * Gets the backing PWM handle.
   *
   * @return The pwm handle.
   */
  public int getPwmHandle() {
    return m_pwm.getHandle();
  }

  /**
   * Gets the PWM channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_pwm.getChannel();
  }

  /**
   * Optionally eliminate the deadband from a motor controller.
   *
   * @param eliminateDeadband If true, set the motor curve for the motor controller to eliminate the
   *     deadband in the middle of the range. Otherwise, keep the full range without modifying any
   *     values.
   */
  public void enableDeadbandElimination(boolean eliminateDeadband) {
    m_pwm.enableDeadbandElimination(eliminateDeadband);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Motor Controller");
    builder.setActuator(true);
    builder.setSafeState(this::disable);
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}

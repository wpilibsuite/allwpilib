// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.MotorSafety;
import edu.wpi.first.wpilibj.PWM;

/** Nidec Brushless Motor. */
public class NidecBrushless extends MotorSafety
    implements MotorController, Sendable, AutoCloseable {
  private boolean m_isInverted;
  private final DigitalOutput m_dio;
  private final PWM m_pwm;
  private volatile double m_speed;
  private volatile boolean m_disabled;

  /**
   * Constructor.
   *
   * @param pwmChannel The PWM channel that the Nidec Brushless controller is attached to. 0-9 are
   *     on-board, 10-19 are on the MXP port
   * @param dioChannel The DIO channel that the Nidec Brushless controller is attached to. 0-9 are
   *     on-board, 10-25 are on the MXP port
   */
  public NidecBrushless(final int pwmChannel, final int dioChannel) {
    setSafetyEnabled(false);

    // the dio controls the output (in PWM mode)
    m_dio = new DigitalOutput(dioChannel);
    SendableRegistry.addChild(this, m_dio);
    m_dio.setPWMRate(15625);
    m_dio.enablePWM(0.5);

    // the pwm enables the controller
    m_pwm = new PWM(pwmChannel);
    SendableRegistry.addChild(this, m_pwm);

    HAL.report(tResourceType.kResourceType_NidecBrushless, pwmChannel + 1);
    SendableRegistry.addLW(this, "Nidec Brushless", pwmChannel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    m_dio.close();
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
    if (!m_disabled) {
      m_speed = speed;
      m_dio.updateDutyCycle(0.5 + 0.5 * (m_isInverted ? -speed : speed));
      m_pwm.setRaw(0xffff);
    }

    feed();
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return m_speed;
  }

  @Override
  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  @Override
  public boolean getInverted() {
    return m_isInverted;
  }

  /**
   * Stop the motor. This is called by the MotorSafety object when it has a timeout for this PWM and
   * needs to stop it from running. Calling set() will re-enable the motor.
   */
  @Override
  public void stopMotor() {
    m_dio.updateDutyCycle(0.5);
    m_pwm.setDisabled();
  }

  @Override
  public String getDescription() {
    return "Nidec " + getChannel();
  }

  /** Disable the motor. The enable() function must be called to re-enable the motor. */
  @Override
  public void disable() {
    m_disabled = true;
    m_dio.updateDutyCycle(0.5);
    m_pwm.setDisabled();
  }

  /**
   * Re-enable the motor after disable() has been called. The set() function must be called to set a
   * new motor speed.
   */
  public void enable() {
    m_disabled = false;
  }

  /**
   * Gets the channel number associated with the object.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_pwm.getChannel();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Nidec Brushless");
    builder.setActuator(true);
    builder.setSafeState(this::stopMotor);
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}

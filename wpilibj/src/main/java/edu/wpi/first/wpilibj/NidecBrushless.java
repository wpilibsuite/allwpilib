/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Nidec Brushless Motor.
 */
public class NidecBrushless extends MotorSafety implements SpeedController, Sendable,
    AutoCloseable {
  private boolean m_isInverted;
  private final DigitalOutput m_dio;
  private final PWM m_pwm;
  private volatile double m_speed;
  private volatile boolean m_disabled;

  private final SendableImpl m_sendableImpl;

  /**
   * Constructor.
   *
   * @param pwmChannel The PWM channel that the Nidec Brushless controller is attached to.
   *                   0-9 are on-board, 10-19 are on the MXP port
   * @param dioChannel The DIO channel that the Nidec Brushless controller is attached to.
   *                   0-9 are on-board, 10-25 are on the MXP port
   */
  public NidecBrushless(final int pwmChannel, final int dioChannel) {
    m_sendableImpl = new SendableImpl(true);

    setSafetyEnabled(false);

    // the dio controls the output (in PWM mode)
    m_dio = new DigitalOutput(dioChannel);
    addChild(m_dio);
    m_dio.setPWMRate(15625);
    m_dio.enablePWM(0.5);

    // the pwm enables the controller
    m_pwm = new PWM(pwmChannel);
    addChild(m_pwm);

    HAL.report(tResourceType.kResourceType_NidecBrushless, pwmChannel);
    setName("Nidec Brushless", pwmChannel);
  }

  @Override
  public void close() {
    m_sendableImpl.close();
    m_dio.close();
    m_pwm.close();
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
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  @Override
  public void pidWrite(double output) {
    set(output);
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

  /**
   * Disable the motor.  The enable() function must be called to re-enable
   * the motor.
   */
  @Override
  public void disable() {
    m_disabled = true;
    m_dio.updateDutyCycle(0.5);
    m_pwm.setDisabled();
  }

  /**
   * Re-enable the motor after disable() has been called.  The set()
   * function must be called to set a new motor speed.
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

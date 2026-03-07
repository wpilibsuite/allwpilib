// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

import java.util.ArrayList;
import org.wpilib.hardware.discrete.PWM;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.hardware.hal.SimDevice.Direction;
import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.system.RobotController;
import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.util.sendable.SendableRegistry;

/** Common base class for all PWM Motor Controllers. */
@SuppressWarnings("removal")
public abstract class PWMMotorController extends MotorSafety
    implements MotorController, Sendable, AutoCloseable {
  private boolean m_isInverted;
  private final ArrayList<PWMMotorController> m_followers = new ArrayList<>();

  /** PWM instances for motor controller. */
  protected PWM m_pwm;

  private SimDevice m_simDevice;
  private SimDouble m_simDutyCycle;

  private boolean m_eliminateDeadband;
  private int m_minPwm;
  private int m_deadbandMinPwm;
  private int m_centerPwm;
  private int m_deadbandMaxPwm;
  private int m_maxPwm;

  /**
   * Constructor.
   *
   * @param name Name to use for SendableRegistry
   * @param channel The PWM channel that the controller is attached to. 0-9 are on-board, 10-19 are
   *     on the MXP port
   */
  @SuppressWarnings("this-escape")
  protected PWMMotorController(final String name, final int channel) {
    m_pwm = new PWM(channel, false);
    SendableRegistry.add(this, name, channel);

    m_simDevice = SimDevice.create("PWMMotorController", channel);
    if (m_simDevice != null) {
      m_simDutyCycle = m_simDevice.createDouble("DutyCycle", Direction.kOutput, 0.0);
      m_pwm.setSimDevice(m_simDevice);
    }
  }

  /** Free the resource associated with the PWM channel and set the value to 0. */
  @Override
  public void close() {
    SendableRegistry.remove(this);
    m_pwm.close();

    if (m_simDevice != null) {
      m_simDevice.close();
      m_simDevice = null;
      m_simDutyCycle = null;
    }
  }

  private int getMinPositivePwm() {
    if (m_eliminateDeadband) {
      return m_deadbandMaxPwm;
    } else {
      return m_centerPwm + 1;
    }
  }

  private int getMaxNegativePwm() {
    if (m_eliminateDeadband) {
      return m_deadbandMinPwm;
    } else {
      return m_centerPwm - 1;
    }
  }

  private int getPositiveScaleFactor() {
    return m_maxPwm - getMinPositivePwm();
  }

  private int getNegativeScaleFactor() {
    return getMaxNegativePwm() - m_minPwm;
  }

  /**
   * Takes a duty cycle from -1 to 1 (the sign indicates direction), and outputs it in the
   * microsecond format.
   *
   * @param dutyCycle The duty cycle between -1 and 1 (sign indicates direction).
   */
  protected final void setDutyCycleInternal(double dutyCycle) {
    if (Double.isFinite(dutyCycle)) {
      dutyCycle = Math.clamp(dutyCycle, -1.0, 1.0);
    } else {
      dutyCycle = 0.0;
    }

    if (m_simDutyCycle != null) {
      m_simDutyCycle.set(dutyCycle);
    }

    int rawValue;
    if (dutyCycle == 0.0) {
      rawValue = m_centerPwm;
    } else if (dutyCycle > 0.0) {
      rawValue = (int) Math.round(dutyCycle * getPositiveScaleFactor()) + getMinPositivePwm();
    } else {
      rawValue = (int) Math.round(dutyCycle * getNegativeScaleFactor()) + getMaxNegativePwm();
    }

    m_pwm.setPulseTimeMicroseconds(rawValue);
  }

  /**
   * Gets the duty cycle from -1 to 1 (the sign indicates direction), from the currently set pulse
   * time.
   *
   * @return motor controller duty cycle
   */
  protected final double getDutyCycleInternal() {
    int rawValue = m_pwm.getPulseTimeMicroseconds();

    if (rawValue == 0) {
      return 0.0;
    } else if (rawValue > m_maxPwm) {
      return 1.0;
    } else if (rawValue < m_minPwm) {
      return -1.0;
    } else if (rawValue > getMinPositivePwm()) {
      return (rawValue - getMinPositivePwm()) / (double) getPositiveScaleFactor();
    } else if (rawValue < getMaxNegativePwm()) {
      return (rawValue - getMaxNegativePwm()) / (double) getNegativeScaleFactor();
    } else {
      return 0.0;
    }
  }

  /**
   * Sets the bounds in microseconds for the controller.
   *
   * @param maxPwm maximum
   * @param deadbandMaxPwm deadband max
   * @param centerPwm center
   * @param deadbandMinPwm deadmand min
   * @param minPwm minimum
   */
  protected final void setBoundsMicroseconds(
      int maxPwm, int deadbandMaxPwm, int centerPwm, int deadbandMinPwm, int minPwm) {
    m_maxPwm = maxPwm;
    m_deadbandMaxPwm = deadbandMaxPwm;
    m_centerPwm = centerPwm;
    m_deadbandMinPwm = deadbandMinPwm;
    m_minPwm = minPwm;
  }

  @Override
  public void setDutyCycle(double dutyCycle) {
    if (m_isInverted) {
      dutyCycle = -dutyCycle;
    }
    setDutyCycleInternal(dutyCycle);

    for (var follower : m_followers) {
      follower.setDutyCycle(dutyCycle);
    }

    feed();
  }

  @Override
  public double getDutyCycle() {
    return getDutyCycleInternal() * (m_isInverted ? -1.0 : 1.0);
  }

  /**
   * Gets the voltage output of the motor controller, nominally between -12 V and 12 V.
   *
   * @return The voltage of the motor controller, nominally between -12 V and 12 V.
   */
  public double getVoltage() {
    return getDutyCycle() * RobotController.getBatteryVoltage();
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

    if (m_simDutyCycle != null) {
      m_simDutyCycle.set(0.0);
    }

    for (var follower : m_followers) {
      follower.disable();
    }
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
    m_eliminateDeadband = eliminateDeadband;
  }

  /**
   * Make the given PWM motor controller follow the output of this one.
   *
   * @param follower The motor controller follower.
   */
  public void addFollower(PWMMotorController follower) {
    m_followers.add(follower);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Motor Controller");
    builder.setActuator(true);
    builder.addDoubleProperty("Value", this::getDutyCycle, this::setDutyCycle);
  }
}

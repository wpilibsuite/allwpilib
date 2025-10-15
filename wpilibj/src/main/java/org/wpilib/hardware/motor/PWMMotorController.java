// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.MotorSafety;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.RobotController;
import java.util.ArrayList;

/** Common base class for all PWM Motor Controllers. */
@SuppressWarnings("removal")
public abstract class PWMMotorController extends MotorSafety
    implements MotorController, Sendable, AutoCloseable {
  private boolean m_isInverted;
  private final ArrayList<PWMMotorController> m_followers = new ArrayList<>();

  /** PWM instances for motor controller. */
  protected PWM m_pwm;

  private SimDevice m_simDevice;
  private SimDouble m_simSpeed;

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
      m_simSpeed = m_simDevice.createDouble("Speed", Direction.kOutput, 0.0);
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
      m_simSpeed = null;
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
   * Takes a speed from -1 to 1, and outputs it in the microsecond format.
   *
   * @param speed the speed to output
   */
  protected final void setSpeed(double speed) {
    if (Double.isFinite(speed)) {
      speed = Math.clamp(speed, -1.0, 1.0);
    } else {
      speed = 0.0;
    }

    if (m_simSpeed != null) {
      m_simSpeed.set(speed);
    }

    int rawValue;
    if (speed == 0.0) {
      rawValue = m_centerPwm;
    } else if (speed > 0.0) {
      rawValue = (int) Math.round(speed * getPositiveScaleFactor()) + getMinPositivePwm();
    } else {
      rawValue = (int) Math.round(speed * getNegativeScaleFactor()) + getMaxNegativePwm();
    }

    m_pwm.setPulseTimeMicroseconds(rawValue);
  }

  /**
   * Gets the speed from -1 to 1, from the currently set pulse time.
   *
   * @return motor controller speed
   */
  protected final double getSpeed() {
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
    if (m_isInverted) {
      speed = -speed;
    }
    setSpeed(speed);

    for (var follower : m_followers) {
      follower.set(speed);
    }

    feed();
  }

  /**
   * Get the recently set value of the PWM. This value is affected by the inversion property.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return getSpeed() * (m_isInverted ? -1.0 : 1.0);
  }

  /**
   * Gets the voltage output of the motor controller, nominally between -12 V and 12 V.
   *
   * @return The voltage of the motor controller, nominally between -12 V and 12 V.
   */
  public double getVoltage() {
    return get() * RobotController.getBatteryVoltage();
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

    if (m_simSpeed != null) {
      m_simSpeed.set(0.0);
    }

    for (var follower : m_followers) {
      follower.disable();
    }
  }

  @Override
  public void stopMotor() {
    // Don't use set(0) as that will feed the watch kitty
    m_pwm.setPulseTimeMicroseconds(0);

    for (var follower : m_followers) {
      follower.stopMotor();
    }
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
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}

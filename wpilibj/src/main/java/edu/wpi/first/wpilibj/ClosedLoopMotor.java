/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A class that performs closed-loop velocity control on a motor.
 *
 * <p>It implements SpeedController so this control can be used transparently within drive bases.
 *
 * <p>Call either setDisplacementRange() or setVelocityRange(), then enable() before use.
 */
public class ClosedLoopMotor extends SendableBase implements SpeedController {
  public static final double kDefaultPeriod = 0.05;

  private PIDController m_controller;
  private double m_minimumInput = -1.0;
  private double m_maximumInput = 1.0;
  private boolean m_isInverted;

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, Kd, and Kff.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param Kff    The feedforward coefficient.
   * @param source The PIDSource object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   * @param period The loop time for doing calculations.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff, PIDSource source,
      SpeedController motor, double period) {
    m_controller = new PIDController(Kp, Ki, Kd, Kff, source, motor, period);
    m_controller.setPIDSourceType(PIDSourceType.kRate);
    addChild(m_controller);
  }

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param source The PIDSource object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   * @param period The loop time for doing calculations.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, PIDSource source, SpeedController motor,
      double period) {
    this(Kp, Ki, Kd, 0.0, source, motor, period);
  }

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param source The PIDSource object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, PIDSource source, SpeedController motor) {
    this(Kp, Ki, Kd, 0.0, source, motor, kDefaultPeriod);
  }

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, Kd, and Kff.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param Kff    The feedforward coefficient.
   * @param source The PIDSource object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff, PIDSource source,
      SpeedController motor) {
    this(Kp, Ki, Kd, Kff, source, motor, kDefaultPeriod);
  }

  /**
   * When Set() is called, its [-1..1] range is mapped to the given displacement range before being
   * set as the controller's reference.
   *
   * <p>This enables position control internally.
   *
   * @param minimumDisplacement Set(-1.0) maps to this value.
   * @param maximumDisplacement Set(1.0) maps to this value.
   */
  public void setDisplacementRange(double minimumDisplacement, double maximumDisplacement) {
    setDisplacementRange(minimumDisplacement, maximumDisplacement, false);
  }

  /**
   * When Set() is called, its [-1..1] range is mapped to the given displacement range before being
   * set as the controller's reference.
   *
   * <p>This enables position control internally.
   *
   * @param minimumDisplacement Set(-1.0) maps to this value.
   * @param maximumDisplacement Set(1.0) maps to this value.
   * @param continuous          Whether the position control should consider the input continuous.
   */
  public void setDisplacementRange(double minimumDisplacement, double maximumDisplacement,
      boolean continuous) {
    m_minimumInput = minimumDisplacement;
    m_maximumInput = maximumDisplacement;
    m_controller.setPIDSourceType(PIDSourceType.kDisplacement);
    m_controller.setInputRange(m_minimumInput, m_maximumInput);
    m_controller.setContinuous(continuous);
  }

  /**
   * When Set() is called, its [-1..1] range is mapped to the given velocity range before being set
   * as the controller's reference.
   *
   * @param minimumVelocity Set(-1.0) maps to this value.
   * @param maximumVelocity Set(1.0) maps to this value.
   */
  public void setVelocityRange(double minimumVelocity, double maximumVelocity) {
    m_minimumInput = minimumVelocity;
    m_maximumInput = maximumVelocity;
    m_controller.setPIDSourceType(PIDSourceType.kRate);
    m_controller.setInputRange(m_minimumInput, m_maximumInput);
  }

  /**
   * Enables the internal controller.
   */
  public void enable() {
    m_controller.enable();
  }

  /**
   * Returns the internal controller.
   */
  public PIDController getController() {
    return m_controller;
  }

  @Override
  public void set(double speed) {
    if (m_isInverted) {
      speed = -speed;
    }

    m_controller.setSetpoint(m_minimumInput + (m_maximumInput - m_minimumInput)
        * (speed + 1.0) / 2.0);
  }

  /**
   * Converts the internal controller's reference to a SpeedController value in the range [-1..1]
   * using the controller input range, then returns it.
   */
  @Override
  public double get() {
    return 2.0 * (m_controller.getSetpoint() - m_minimumInput) / (m_maximumInput - m_minimumInput)
        - 1.0;
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
    stopMotor();
  }

  @Override
  public void stopMotor() {
    m_controller.disable();
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController.
   */
  @Override
  public void pidWrite(double output) {
    set(output);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("ClosedLoopMotor");
    builder.setSafeState(m_controller::reset);
  }
}

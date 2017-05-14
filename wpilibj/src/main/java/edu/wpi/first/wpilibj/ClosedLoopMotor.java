/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.ctrlsys.GainNode;
import edu.wpi.first.wpilibj.ctrlsys.INode;
import edu.wpi.first.wpilibj.ctrlsys.Output;
import edu.wpi.first.wpilibj.ctrlsys.PIDNode;
import edu.wpi.first.wpilibj.ctrlsys.RefInput;
import edu.wpi.first.wpilibj.ctrlsys.SumNode;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A class that performs closed-loop control on a motor.
 *
 * <p>It implements SpeedController so this control can be used transparently within drive bases.
 *
 * <p>Call either setDisplacmentRange() or setVelocityRange(), then enable() before use.
 */
public class ClosedLoopMotor extends SendableBase implements SpeedController {
  public static final double kDefaultPeriod = 0.05;

  private RefInput m_refInput = new RefInput(0.0);
  private GainNode m_feedforward;
  private SumNode m_sum;
  private PIDNode m_pid;
  private Output m_output;

  private double m_minimumInput = -1.0;
  private double m_maximumInput = 1.0;
  private boolean m_isInverted = false;

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, Kd, and Kff.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param Kff    The feedforward coefficient.
   * @param input  The INode object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   * @param period The loop time for doing calculations.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff, INode input,
      SpeedController motor, double period) {
    m_feedforward = new GainNode(Kff, m_refInput);
    m_sum = new SumNode(m_refInput, true, input, false);
    m_pid = new PIDNode(Kp, Ki, Kd, m_feedforward, m_sum, period);
    m_output = new Output(m_pid, motor, period);
  }

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param input  The INode object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   * @param period The loop time for doing calculations.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, INode input, SpeedController motor,
      double period) {
    m_sum = new SumNode(m_refInput, true, input, false);
    m_pid = new PIDNode(Kp, Ki, Kd, m_sum, period);
    m_output = new Output(m_pid, motor, period);
  }

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param input  The INode object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, INode input, SpeedController motor) {
    this(Kp, Ki, Kd, 0.0, input, motor, kDefaultPeriod);
  }

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, Kd, and Kff.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param Kff    The feedforward coefficient.
   * @param input  The INode object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   */
  @SuppressWarnings("ParameterName")
  public ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff, INode input,
      SpeedController motor) {
    this(Kp, Ki, Kd, Kff, input, motor, kDefaultPeriod);
  }

  /**
   * When Set() is called, its [-1..1] range is mapped to the given displacement range before being
   * set as the controller's reference.
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
   * @param minimumDisplacement Set(-1.0) maps to this value.
   * @param maximumDisplacement Set(1.0) maps to this value.
   * @param continuous          Whether the position control should consider the input continuous.
   */
  public void setDisplacementRange(double minimumDisplacement, double maximumDisplacement,
        boolean continuous) {
    m_minimumInput = minimumDisplacement;
    m_maximumInput = maximumDisplacement;
    m_sum.setInputRange(m_minimumInput, m_maximumInput);
    m_sum.setContinuous(continuous);
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
    m_sum.setInputRange(m_minimumInput, m_maximumInput);
  }

  /**
   * Enables the internal controller.
   */
  public void enable() {
    m_output.enable();
  }

  /**
   * Returns the internal controller.
   */
  public PIDNode getController() {
    return m_pid;
  }

  @Override
  public void set(double speed) {
    if (m_isInverted) {
      speed = -speed;
    }

    m_refInput.set(m_minimumInput + (m_maximumInput - m_minimumInput) * (speed + 1.0) / 2.0);
  }

  /**
   * Converts the internal controller's reference to a SpeedController value in the range [-1..1]
   * using the controller input range, then returns it.
   */
  @Override
  public double get() {
    return 2.0 * (m_refInput.getOutput() - m_minimumInput) / (m_maximumInput - m_minimumInput)
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
    m_output.disable();
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
    builder.setSafeState(() -> {
      m_output.disable();
      m_pid.reset();
    });
  }
}

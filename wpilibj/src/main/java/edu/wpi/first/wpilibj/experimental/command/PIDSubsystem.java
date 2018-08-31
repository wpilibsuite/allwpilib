/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.experimental.controller.ControllerOutput;
import edu.wpi.first.wpilibj.experimental.controller.ControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.MeasurementSource;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

/**
 * This class is designed to handle the case where there is a Subsystem which uses a single
 * PIDController almost constantly (for instance, an elevator which attempts to stay at a constant
 * height).
 *
 * <p>It provides some convenience methods to run an internal PIDController. It also allows access
 * to the internal PIDController in order to give total control to the programmer.
 */
public abstract class PIDSubsystem extends Subsystem implements ControllerOutput,
       MeasurementSource {
  // The internal PIDController
  private final PIDController m_controller;
  private final ControllerRunner m_runner;

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * @param name The name.
   * @param Kp   The proportional value.
   * @param Ki   The integral value.
   * @param Kd   The derivative value.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(String name, double Kp, double Ki, double Kd) {
    super(name);
    m_controller = new PIDController(Kp, Ki, Kd, this);
    m_runner = new ControllerRunner(m_controller, this);
    addChild("PIDController", m_controller);
  }

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * @param name        The name.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedforward function.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(String name, double Kp, double Ki, double Kd,
                      DoubleSupplier feedforward) {
    super(name);
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this);
    m_runner = new ControllerRunner(m_controller, this);
    addChild("PIDController", m_controller);
  }

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * <p>It will also space the time between PID loop calculations to be equal to the given period.
   *
   * @param name        The name.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedfoward function.
   * @param period      The time (in seconds) between calculations.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(String name, double Kp, double Ki, double Kd,
                      DoubleSupplier feedforward, double period) {
    super(name);
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this, period);
    m_runner = new ControllerRunner(m_controller, this);
    addChild("PIDController", m_controller);
  }

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * <p>It will use the class name as its name.
   *
   * @param Kp The proportional value.
   * @param Ki The integral value.
   * @param Kd The derivative value.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(double Kp, double Ki, double Kd) {
    m_controller = new PIDController(Kp, Ki, Kd, this);
    m_runner = new ControllerRunner(m_controller, this);
    addChild("PIDController", m_controller);
  }

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * <p>It will use the class name as its name.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedforward function.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(double Kp, double Ki, double Kd,
                      DoubleSupplier feedforward) {
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this);
    m_runner = new ControllerRunner(m_controller, this);
    addChild("PIDController", m_controller);
  }

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * <p>It will use the class name as its name. It will also space the time between PID loop
   * calculations to be equal to the given period.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedforward function.
   * @param period      The time (in seconds) between calculations.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(double Kp, double Ki, double Kd,
                      DoubleSupplier feedforward, double period) {
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this, period);
    m_runner = new ControllerRunner(m_controller, this);
    addChild("PIDController", m_controller);
  }

  /**
   * Enables the internal PIDController.
   */
  public void enable() {
    m_runner.disable();
  }

  /**
   * Disables the internal PIDController.
   */
  public void disable() {
    m_runner.disable();
  }

  // ControllerOutput interface
  @Override
  public abstract void setOutput(double output);

  // MeasurementSource interface
  @Override
  public abstract double getMeasurement();

  /**
   * Sets the reference to the given value.
   *
   * <p>If setRange() was called, then the given setpoint will be trimmed to fit within the range.
   *
   * @param reference The new reference.
   */
  public void setReference(double reference) {
    m_controller.setReference(reference);
  }

  /**
   * Adds the given value to the reference.
   *
   * <p>If setRange() was used, then the bounds will still be honored by this method.
   *
   * @param deltaReference The change in the reference.
   */
  public void setReferenceRelative(double deltaReference) {
    setReference(getReference() + deltaReference);
  }

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the output.
   */
  public void setInputRange(double minimumInput, double maximumInput) {
    m_controller.setInputRange(minimumInput, maximumInput);
  }

  /**
   * Sets the maximum and minimum values to write.
   *
   * @param minimumOutput The minimum value to write to the output.
   * @param maximumOutput The maximum value to write to the output.
   */
  public void setOutputRange(double minimumOutput, double maximumOutput) {
    m_controller.setOutputRange(minimumOutput, maximumOutput);
  }

  /**
   * Return the current reference.
   *
   * @return The current reference.
   */
  public double getReference() {
    return m_controller.getReference();
  }

  /**
   * Set the absolute error which is considered tolerable for use with atReference().
   *
   * @param tolerance      Absolute error which is tolerable.
   */
  public void setAbsoluteTolerance(double tolerance) {
    m_controller.setAbsoluteTolerance(tolerance);
  }

  /**
   * Set the absolute error which is considered tolerable for use with atReference().
   *
   * @param tolerance      Absolute error which is tolerable.
   * @param deltaTolerance Change in absolute error per second which is tolerable.
   */
  public void setAbsoluteTolerance(double tolerance, double deltaTolerance) {
    m_controller.setAbsoluteTolerance(tolerance, deltaTolerance);
  }

  /**
   * Set the percent error which is considered tolerable for use with atReference().
   *
   * @param tolerance Percent error which is tolerable.
   */
  public void setPercentTolerance(double tolerance) {
    m_controller.setPercentTolerance(tolerance);
  }

  /**
   * Set the percent error which is considered tolerable for use with atReference().
   *
   * @param tolerance      Percent error which is tolerable.
   * @param deltaTolerance Change in percent error per second which is tolerable.
   */
  public void setPercentTolerance(double tolerance, double deltaTolerance) {
    m_controller.setPercentTolerance(tolerance, deltaTolerance);
  }

  /**
   * Return true if the error is within the percentage of the total input range, determined by
   * setTolerance().
   *
   * <p>This asssumes that the maximum and minimum input were set using setInputRange(). Use
   * atReference() in the isFinished() method of commands that use this subsystem.
   */
  public boolean atReference() {
    return m_controller.atReference();
  }

  /**
   * Returns the PIDController used by this PIDSubsystem.
   *
   * <p>Use this if you would like to fine tune the PID loop.
   *
   * @return The PIDController used by this PIDSubsystem
   */
  protected PIDController getPIDController() {
    return m_controller;
  }
}

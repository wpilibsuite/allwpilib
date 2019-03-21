/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

/**
 * This class is designed to handle the case where there is a Subsystem which uses a single
 * PIDController almost constantly (for instance, an elevator which attempts to stay at a constant
 * height).
 *
 * <p>It provides some convenience methods to run an internal PIDController. It also allows access
 * to the internal PIDController in order to give total control to the programmer.
 */
public abstract class PIDSubsystem extends Subsystem {
  // The internal PIDController
  private final PIDController m_controller;
  private final AsynchronousControllerRunner m_runner;
  private double m_reference = 0;

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp   The proportional value.
   * @param Ki   The integral value.
   * @param Kd   The derivative value.
   */
  @SuppressWarnings("ParameterName")
  public PIDSubsystem(double Kp, double Ki, double Kd) {
    m_controller = new PIDController(Kp, Ki, Kd);
    m_runner = new AsynchronousControllerRunner(
        m_controller, this::getReference, this::getMeasurement, this::setOutput);
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

  public abstract void setOutput(double output);

  public abstract double getMeasurement();

  /**
   * Sets the reference to the given value.
   *
   * <p>If setRange() was called, then the given setpoint will be trimmed to fit within the range.
   *
   * @param reference The new reference.
   */
  public synchronized void setReference(double reference) {
    m_reference = reference;
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
  public synchronized double getReference() {
    return m_reference;
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

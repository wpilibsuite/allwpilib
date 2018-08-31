/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.experimental.controller.ControllerOutput;
import edu.wpi.first.wpilibj.experimental.controller.ControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.MeasurementSource;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

public abstract class PIDCommand extends Command implements ControllerOutput, MeasurementSource {
  // The internal PIDController
  private final PIDController m_controller;
  private final ControllerRunner m_runner;

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param name The name of the command.
   * @param Kp   The proportional value.
   * @param Ki   The integral value.
   * @param Kd   The derivative value.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(String name, double Kp, double Ki, double Kd) {
    super(name);
    m_controller = new PIDController(Kp, Ki, Kd, this);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param name   The name of the command.
   * @param Kp     The proportional value.
   * @param Ki     The integral value.
   * @param Kd     The derivative value.
   * @param period The period between controller updates in seconds. The default
   *               is 5ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(String name, double Kp, double Ki, double Kd,
                    double period) {
    super(name);
    m_controller = new PIDController(Kp, Ki, Kd, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param name        The name of the command.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(String name, double Kp, double Ki, double Kd, DoubleSupplier feedforward,
                    double period) {
    super(name);
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp The proportional value.
   * @param Ki The integral value.
   * @param Kd The derivative value.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd) {
    m_controller = new PIDController(Kp, Ki, Kd, this);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp     The proportional value.
   * @param Ki     The integral value.
   * @param Kd     The derivative value.
   * @param period The period between controller updates in seconds. The default
   *               is 5ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd, double period) {
    m_controller = new PIDController(Kp, Ki, Kd, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd, DoubleSupplier feedforward, double period) {
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param name      The name of the command.
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param subsystem The subsystem that this command requires.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(String name, double Kp, double Ki, double Kd, Subsystem subsystem) {
    super(name, subsystem);
    m_controller = new PIDController(Kp, Ki, Kd, this);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param name      The name of the command.
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param period    The period between controller updates in seconds. The
   *                  default is 5ms.
   * @param subsystem The subsystem that this command requires.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(String name, double Kp, double Ki, double Kd, double period,
                    Subsystem subsystem) {
    super(name, subsystem);
    m_controller = new PIDController(Kp, Ki, Kd, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param name        The name of the command.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   * @param subsystem   The subsystem that this command requires.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(String name, double Kp, double Ki, double Kd, DoubleSupplier feedforward,
                    double period, Subsystem subsystem) {
    super(name, subsystem);
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param subsystem The subsystem that this command requires.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd, Subsystem subsystem) {
    super(subsystem);
    m_controller = new PIDController(Kp, Ki, Kd, this);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param period    The period between controller updates in seconds. The
   *                  default is 5ms.
   * @param subsystem The subsystem that this command requires.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd, double period, Subsystem subsystem) {
    super(subsystem);
    m_controller = new PIDController(Kp, Ki, Kd, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   * @param subsystem   The subsystem that this command requires.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd, DoubleSupplier feedforward, double period,
                    Subsystem subsystem) {
    super(subsystem);
    m_controller = new PIDController(Kp, Ki, Kd, feedforward, this, period);
    m_runner = new ControllerRunner(m_controller, this);
  }

  /**
   * Adds the given value to the reference. If {@link PIDController#setInputRange(double, double)
   * setInputRange(...)} was used, then the bounds will still be honored by this method.
   *
   * @param deltaReference The change in the reference.
   */
  public void setReferenceRelative(double deltaReference) {
    setReference(getReference() + deltaReference);
  }

  // ControllerOutput interface
  @Override
  public abstract void setOutput(double output);

  // MeasurementSource interface
  @Override
  public abstract double getMeasurement();

  @Override
  public void initSendable(SendableBuilder builder) {
    m_controller.initSendable(builder);
    super.initSendable(builder);
    builder.setSmartDashboardType("PIDCommand");
  }

  /**
   * Returns the {@link PIDController} used by this {@link PIDCommand}. Use this if you would
   * like to fine-tune the pid loop.
   *
   * @return The {@link PIDController} used by this {@link PIDCommand}.
   */
  protected PIDController getPIDController() {
    return m_controller;
  }

  @Override
  @SuppressWarnings("MethodName")
  protected void _initialize() {
    m_runner.enable();
  }

  @Override
  @SuppressWarnings("MethodName")
  protected void _interrupted() {
    _end();
  }

  @Override
  @SuppressWarnings("MethodName")
  protected void _end() {
    m_runner.disable();
  }

  /**
   * Set the reference for the PIDController.
   *
   * @param reference The desired reference.
   */
  protected void setReference(double reference) {
    m_controller.setReference(reference);
  }

  /**
   * Returns the current reference of the PIDController.
   *
   * @return The current reference.
   */
  protected double getReference() {
    return m_controller.getReference();
  }
}

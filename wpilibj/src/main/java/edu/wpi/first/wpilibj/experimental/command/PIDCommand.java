/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

public abstract class PIDCommand extends Command {
  // The internal PIDController
  private final PIDController m_controller;
  private final AsynchronousControllerRunner m_runner;
  private double m_reference = 0;

  /**
   * Instantiates a {@link PIDCommand} that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp   The proportional value.
   * @param Ki   The integral value.
   * @param Kd   The derivative value.
   */
  @SuppressWarnings("ParameterName")
  public PIDCommand(double Kp, double Ki, double Kd) {
    m_controller = new PIDController(Kp, Ki, Kd);
    m_runner = new AsynchronousControllerRunner(
        m_controller, this::getReference, this::getMeasurement, this::setOutput);
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

  public abstract void setOutput(double output);

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
  protected synchronized void setReference(double reference) {
    m_reference = reference;
  }

  /**
   * Returns the current reference of the PIDController.
   *
   * @return The current reference.
   */
  protected synchronized double getReference() {
    return m_reference;
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.controller.PIDController;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

/**
 * A command that controls an output with a {@link PIDController}. Runs forever by default - to add
 * exit conditions and/or other behavior, subclass this class. The controller calculation and output
 * are performed synchronously in the command's execute() method.
 */
public class PIDCommand extends CommandBase {
  private final PIDController m_controller;
  private final DoubleSupplier m_measurement;
  private final DoubleSupplier m_setpoint;
  private final DoubleConsumer m_useOutput;

  /**
   * Creates a new PIDCommand, which controls the given output with a PIDController.
   *
   * @param controller the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param setpointSource the controller's setpoint
   * @param useOutput the controller's output
   * @param requirements the subsystems required by this command
   */
  public PIDCommand(
      PIDController controller,
      DoubleSupplier measurementSource,
      DoubleSupplier setpointSource,
      DoubleConsumer useOutput,
      Subsystem... requirements) {
    requireNonNullParam(controller, "controller", "SynchronousPIDCommand");
    requireNonNullParam(measurementSource, "measurementSource", "SynchronousPIDCommand");
    requireNonNullParam(setpointSource, "setpointSource", "SynchronousPIDCommand");
    requireNonNullParam(useOutput, "useOutput", "SynchronousPIDCommand");

    m_controller = controller;
    m_useOutput = useOutput;
    m_measurement = measurementSource;
    m_setpoint = setpointSource;
    addRequirements(requirements);
  }

  /**
   * Creates a new PIDCommand, which controls the given output with a PIDController.
   *
   * @param controller the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param setpoint the controller's setpoint
   * @param useOutput the controller's output
   * @param requirements the subsystems required by this command
   */
  public PIDCommand(
      PIDController controller,
      DoubleSupplier measurementSource,
      double setpoint,
      DoubleConsumer useOutput,
      Subsystem... requirements) {
    this(controller, measurementSource, () -> setpoint, useOutput, requirements);
  }

  /**
   * Resets the controller. If you override this, make sure that you call super.
   */
  @Override
  public void initialize() {
    m_controller.reset();
  }

  @Override
  public final void execute() {
    m_useOutput.accept(
        m_controller.calculate(m_measurement.getAsDouble(), m_setpoint.getAsDouble()));
  }

  @Override
  public final void end(boolean interrupted) {
    m_useOutput.accept(0);
  }

  /**
   * Returns the PIDController used by the command.
   *
   * @return The PIDController
   */
  public final PIDController getController() {
    return m_controller;
  }
}

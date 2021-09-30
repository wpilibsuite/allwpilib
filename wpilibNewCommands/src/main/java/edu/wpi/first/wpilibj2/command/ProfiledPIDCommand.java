// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.math.trajectory.TrapezoidProfile.State;
import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.controller.ProfiledPIDController;
import java.util.function.BiConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.Supplier;

/**
 * A command that controls an output with a {@link ProfiledPIDController}. Runs forever by default -
 * to add exit conditions and/or other behavior, subclass this class. The controller calculation and
 * output are performed synchronously in the command's execute() method.
 */
public class ProfiledPIDCommand extends CommandBase {
  private final ProfiledPIDController m_controller;
  private final DoubleSupplier m_measurement;
  private final Supplier<State> m_goal;
  private final BiConsumer<Double, State> m_useOutput;

  /**
   * Creates a new PIDCommand, which controls the given output with a ProfiledPIDController. Goal
   * velocity is specified.
   *
   * @param controller the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goalSource the controller's goal
   * @param useOutput the controller's output
   * @param requirements the subsystems required by this command
   */
  public ProfiledPIDCommand(
      ProfiledPIDController controller,
      DoubleSupplier measurementSource,
      Supplier<State> goalSource,
      BiConsumer<Double, State> useOutput,
      Subsystem... requirements) {
    requireNonNullParam(controller, "controller", "SynchronousPIDCommand");
    requireNonNullParam(measurementSource, "measurementSource", "SynchronousPIDCommand");
    requireNonNullParam(goalSource, "goalSource", "SynchronousPIDCommand");
    requireNonNullParam(useOutput, "useOutput", "SynchronousPIDCommand");

    m_controller = controller;
    m_useOutput = useOutput;
    m_measurement = measurementSource;
    m_goal = goalSource;
    addRequirements(requirements);
  }

  /**
   * Creates a new PIDCommand, which controls the given output with a ProfiledPIDController. Goal
   * velocity is implicitly zero.
   *
   * @param controller the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goalSource the controller's goal
   * @param useOutput the controller's output
   * @param requirements the subsystems required by this command
   */
  public ProfiledPIDCommand(
      ProfiledPIDController controller,
      DoubleSupplier measurementSource,
      DoubleSupplier goalSource,
      BiConsumer<Double, State> useOutput,
      Subsystem... requirements) {
    requireNonNullParam(controller, "controller", "SynchronousPIDCommand");
    requireNonNullParam(measurementSource, "measurementSource", "SynchronousPIDCommand");
    requireNonNullParam(goalSource, "goalSource", "SynchronousPIDCommand");
    requireNonNullParam(useOutput, "useOutput", "SynchronousPIDCommand");

    m_controller = controller;
    m_useOutput = useOutput;
    m_measurement = measurementSource;
    m_goal = () -> new State(goalSource.getAsDouble(), 0);
    addRequirements(requirements);
  }

  /**
   * Creates a new PIDCommand, which controls the given output with a ProfiledPIDController. Goal
   * velocity is specified.
   *
   * @param controller the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goal the controller's goal
   * @param useOutput the controller's output
   * @param requirements the subsystems required by this command
   */
  public ProfiledPIDCommand(
      ProfiledPIDController controller,
      DoubleSupplier measurementSource,
      State goal,
      BiConsumer<Double, State> useOutput,
      Subsystem... requirements) {
    this(controller, measurementSource, () -> goal, useOutput, requirements);
  }

  /**
   * Creates a new PIDCommand, which controls the given output with a ProfiledPIDController. Goal
   * velocity is implicitly zero.
   *
   * @param controller the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goal the controller's goal
   * @param useOutput the controller's output
   * @param requirements the subsystems required by this command
   */
  public ProfiledPIDCommand(
      ProfiledPIDController controller,
      DoubleSupplier measurementSource,
      double goal,
      BiConsumer<Double, State> useOutput,
      Subsystem... requirements) {
    this(controller, measurementSource, () -> goal, useOutput, requirements);
  }

  @Override
  public final void initialize() {
    m_controller.reset(m_measurement.getAsDouble());
  }

  @Override
  public final void execute() {
    m_useOutput.accept(
        m_controller.calculate(m_measurement.getAsDouble(), m_goal.get()),
        m_controller.getSetpoint());
  }

  @Override
  public final void end(boolean interrupted) {
    m_useOutput.accept(0.0, new State());
  }

  /**
   * Returns the ProfiledPIDController used by the command.
   *
   * @return The ProfiledPIDController
   */
  public final ProfiledPIDController getController() {
    return m_controller;
  }
}

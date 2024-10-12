// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.math.trajectory.TrapezoidProfile.State;
import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.math.trajectory.TrapezoidProfile.State;

/**
 * A subsystem that uses a {@link ProfiledPIDController} to control an output. The controller is run
 * synchronously from the subsystem's periodic() method.
 *
 * <p>This class is provided by the NewCommands VendorDep
 *
 * @deprecated Use a ProfiledPIDController instead
 */
@Deprecated(forRemoval = true, since = "2025")
public abstract class ProfiledPIDSubsystem extends SubsystemBase {
  /** Profiled PID controller. */
  protected final ProfiledPIDController m_controller;

  /** Whether the profiled PID controller output is enabled. */
  protected boolean m_enabled;

  /**
   * Creates a new ProfiledPIDSubsystem.
   *
   * @param controller the ProfiledPIDController to use
   * @param initialPosition the initial goal position of the controller
   */
  public ProfiledPIDSubsystem(ProfiledPIDController controller, double initialPosition) {
    m_controller = requireNonNullParam(controller, "controller", "ProfiledPIDSubsystem");
    setGoal(initialPosition);
  }

  /**
   * Creates a new ProfiledPIDSubsystem. Initial goal position is zero.
   *
   * @param controller the ProfiledPIDController to use
   */
  public ProfiledPIDSubsystem(ProfiledPIDController controller) {
    this(controller, 0);
  }

  @Override
  public void periodic() {
    if (m_enabled) {
      useOutput(m_controller.calculate(getMeasurement()), m_controller.getSetpoint());
    }
  }

  /**
   * Returns the ProfiledPIDController.
   *
   * @return The controller.
   */
  public ProfiledPIDController getController() {
    return m_controller;
  }

  /**
   * Sets the goal state for the subsystem.
   *
   * @param goal The goal state for the subsystem's motion profile.
   */
  public final void setGoal(TrapezoidProfile.State goal) {
    m_controller.setGoal(goal);
  }

  /**
   * Sets the goal state for the subsystem. Goal velocity assumed to be zero.
   *
   * @param goal The goal position for the subsystem's motion profile.
   */
  public final void setGoal(double goal) {
    setGoal(new TrapezoidProfile.State(goal, 0));
  }

  /**
   * Uses the output from the ProfiledPIDController.
   *
   * @param output the output of the ProfiledPIDController
   * @param setpoint the setpoint state of the ProfiledPIDController, for feedforward
   */
  protected abstract void useOutput(double output, State setpoint);

  /**
   * Returns the measurement of the process variable used by the ProfiledPIDController.
   *
   * @return the measurement of the process variable
   */
  protected abstract double getMeasurement();

  /** Enables the PID control. Resets the controller. */
  public void enable() {
    m_enabled = true;
    m_controller.reset(getMeasurement());
  }

  /** Disables the PID control. Sets output to zero. */
  public void disable() {
    m_enabled = false;
    useOutput(0, new State());
  }

  /**
   * Returns whether the controller is enabled.
   *
   * @return Whether the controller is enabled.
   */
  public boolean isEnabled() {
    return m_enabled;
  }
}

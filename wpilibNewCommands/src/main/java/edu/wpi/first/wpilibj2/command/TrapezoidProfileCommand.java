// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.math.trajectory.TrapezoidProfile.State;
import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj.Timer;
import java.util.function.Consumer;
import java.util.function.Supplier;

/**
 * A command that runs a {@link TrapezoidProfile}. Useful for smoothly controlling mechanism motion.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class TrapezoidProfileCommand extends Command {
  private final TrapezoidProfile m_profile;
  private final Consumer<State> m_output;
  private final Supplier<State> m_goal;
  private final Supplier<State> m_currentState;
  private final boolean m_newAPI; // TODO: Remove
  private final Timer m_timer = new Timer();

  /**
   * Creates a new TrapezoidProfileCommand that will execute the given {@link TrapezoidProfile}.
   * Output will be piped to the provided consumer function.
   *
   * @param profile The motion profile to execute.
   * @param output The consumer for the profile output.
   * @param goal The supplier for the desired state
   * @param currentState The current state
   * @param requirements The subsystems required by this command.
   */
  @SuppressWarnings("this-escape")
  public TrapezoidProfileCommand(
      TrapezoidProfile profile,
      Consumer<State> output,
      Supplier<State> goal,
      Supplier<State> currentState,
      Subsystem... requirements) {
    m_profile = requireNonNullParam(profile, "profile", "TrapezoidProfileCommand");
    m_output = requireNonNullParam(output, "output", "TrapezoidProfileCommand");
    m_goal = goal;
    m_currentState = currentState;
    m_newAPI = true;
    addRequirements(requirements);
  }

  /**
   * Creates a new TrapezoidProfileCommand that will execute the given {@link TrapezoidProfile}.
   * Output will be piped to the provided consumer function.
   *
   * @param profile The motion profile to execute.
   * @param output The consumer for the profile output.
   * @param requirements The subsystems required by this command.
   * @deprecated The new constructor allows you to pass in a supplier for desired and current state.
   *     This allows you to change goals at runtime.
   */
  @Deprecated(since = "2024", forRemoval = true)
  @SuppressWarnings("this-escape")
  public TrapezoidProfileCommand(
      TrapezoidProfile profile, Consumer<State> output, Subsystem... requirements) {
    m_profile = requireNonNullParam(profile, "profile", "TrapezoidProfileCommand");
    m_output = requireNonNullParam(output, "output", "TrapezoidProfileCommand");
    m_newAPI = false;
    m_goal = null;
    m_currentState = null;
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_timer.restart();
  }

  @Override
  @SuppressWarnings("removal")
  public void execute() {
    if (m_newAPI) {
      m_output.accept(m_profile.calculate(m_timer.get(), m_currentState.get(), m_goal.get()));
    } else {
      m_output.accept(m_profile.calculate(m_timer.get()));
    }
  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();
  }

  @Override
  public boolean isFinished() {
    return m_timer.hasElapsed(m_profile.totalTime());
  }
}

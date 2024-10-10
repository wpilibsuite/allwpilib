// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.math.trajectory.TrapezoidProfile.State;
import java.util.function.Consumer;
import java.util.function.Supplier;

/**
 * A command that runs a {@link TrapezoidProfile}. Useful for smoothly controlling mechanism motion.
 *
 * <p>This class is provided by the NewCommands VendorDep
 *
 * @deprecated Use a TrapezoidProfile instead
 */
@Deprecated(since = "2025", forRemoval = true)
public class TrapezoidProfileCommand extends Command {
  private final TrapezoidProfile m_profile;
  private final Consumer<State> m_output;
  private final Supplier<State> m_goal;
  private final Supplier<State> m_currentState;

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
    addRequirements(requirements);
  }

  @Override
  public void initialize() {}

  @Override
  @SuppressWarnings("removal")
  public void execute() {
    m_output.accept(m_profile.calculate(0.02, m_currentState.get(), m_goal.get()));
  }

  @Override
  public void end(boolean interrupted) {}

  @Override
  public boolean isFinished() {
    return m_profile.isFinished(0);
  }
}

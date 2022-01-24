// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.math.trajectory.TrapezoidProfile.State;
import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj.Timer;
import java.util.function.Consumer;

/**
 * A command that runs a {@link TrapezoidProfile}. Useful for smoothly controlling mechanism motion.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class TrapezoidProfileCommand extends CommandBase {
  private final TrapezoidProfile m_profile;
  private final Consumer<State> m_output;

  private final Timer m_timer = new Timer();

  /**
   * Creates a new TrapezoidProfileCommand that will execute the given {@link TrapezoidProfile}.
   * Output will be piped to the provided consumer function.
   *
   * @param profile The motion profile to execute.
   * @param output The consumer for the profile output.
   * @param requirements The subsystems required by this command.
   */
  public TrapezoidProfileCommand(
      TrapezoidProfile profile, Consumer<State> output, Subsystem... requirements) {
    m_profile = requireNonNullParam(profile, "profile", "TrapezoidProfileCommand");
    m_output = requireNonNullParam(output, "output", "TrapezoidProfileCommand");
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void execute() {
    m_output.accept(m_profile.calculate(m_timer.get()));
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

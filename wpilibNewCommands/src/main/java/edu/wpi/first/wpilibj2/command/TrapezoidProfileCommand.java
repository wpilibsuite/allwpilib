/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.function.Consumer;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

import static edu.wpi.first.wpilibj.trajectory.TrapezoidProfile.State;
import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A command that runs a {@link TrapezoidProfile}.  Useful for smoothly controlling mechanism
 * motion.
 */
public class TrapezoidProfileCommand extends CommandBase {
  private final TrapezoidProfile m_profile;
  private final Consumer<State> m_output;

  private final Timer m_timer = new Timer();

  /**
   * Creates a new TrapezoidProfileCommand that will execute the given {@link TrapezoidProfile}.
   * Output will be piped to the provided consumer function.
   *
   * @param profile      The motion profile to execute.
   * @param output       The consumer for the profile output.
   * @param requirements The subsystems required by this command.
   */
  public TrapezoidProfileCommand(TrapezoidProfile profile,
                                 Consumer<State> output,
                                 Subsystem... requirements) {
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

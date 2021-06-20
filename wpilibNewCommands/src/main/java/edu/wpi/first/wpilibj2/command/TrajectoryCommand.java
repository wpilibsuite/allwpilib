// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.wpilibj.Timer;
import java.util.function.Consumer;

/**
 * A command that follows a certain trajectory, passing the target state to an output function every
 * iteration.
 *
 * <p>This command does <b>not</b> handle Ramsete, PID, and Feedforward control internally - they
 * should be used by the subsystem in the output function.
 */
public class TrajectoryCommand extends CommandBase {
  private final Trajectory m_trajectory;
  private final Consumer<Trajectory.State> m_output;
  private final Timer m_timer = new Timer();

  /**
   * Create a new TrajectoryCommand.
   *
   * @param trajectory The trajectory to follow.
   * @param output A function that consumes the target state for each timestep. This state can be
   *     fed into a Ramsete controller or similar.
   * @param requirements The subsystems to require.
   */
  public TrajectoryCommand(
      Trajectory trajectory, Consumer<Trajectory.State> output, Subsystem... requirements) {
    m_trajectory = requireNonNullParam(trajectory, "trajectory", "TrajectoryCommand");
    m_output = requireNonNullParam(output, "output", "TrajectoryCommand");

    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void execute() {
    m_output.accept(m_trajectory.sample(m_timer.get()));
  }

  @Override
  public boolean isFinished() {
    return m_timer.hasElapsed(m_trajectory.getTotalTimeSeconds());
  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();

    if (interrupted) {
      m_output.accept(new Trajectory.State());
    }
  }
}

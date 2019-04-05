package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.experimental.command.SendableCommandBase;

/**
 * A simple command that grabs a hatch with the {@link HatchSubsystem}.  Written explicitly for
 * pedagogical purposes.  Actual code should inline a command this simple with
 * {@link edu.wpi.first.wpilibj.experimental.command.InstantCommand}.
 */
public class GrabHatch extends SendableCommandBase {

  // The subsystem the command runs on
  private final HatchSubsystem m_hatchSubsystem;

  public GrabHatch(HatchSubsystem subsystem) {
    m_hatchSubsystem = subsystem;
    addRequirements(m_hatchSubsystem);
  }

  @Override
  public void initialize() {
    m_hatchSubsystem.grabHatch();
  }

  @Override
  public boolean isFinished() {
    return true;
  }
}
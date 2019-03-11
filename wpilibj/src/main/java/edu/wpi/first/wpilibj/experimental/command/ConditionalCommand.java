package edu.wpi.first.wpilibj.experimental.command;

import java.util.HashSet;
import java.util.Set;
import java.util.function.BooleanSupplier;

/**
 * Runs one of two commands, depending on the value of the given condition when this command is
 * initialized.  Does not actually schedule the selected command - rather, the command is run
 * through this command; this ensures that the command will behave as expected if used as
 * part of a CommandGroup.  Requires the requirements of both commands, again to ensure proper
 * functioning when used in a CommandGroup.  If this is undesired, consider using
 * {@link ScheduleCommand}.
 */
public class ConditionalCommand extends SendableCommandBase {

  private final Command m_onTrue;
  private final Command m_onFalse;
  private final BooleanSupplier m_condition;
  private Command m_selectedCommand;
  private final Set<Subsystem> m_requirements = new HashSet<>();

  /**
   * Creates a new ConditionalCommand.
   *
   * @param onTrue    the command to run if the condition is true
   * @param onFalse   the command to run if the condition is false
   * @param condition the condition to determine which command to run
   */
  public ConditionalCommand(Command onTrue, Command onFalse, BooleanSupplier condition) {
    m_onTrue = onTrue;
    m_onFalse = onFalse;
    m_condition = condition;
    m_requirements.addAll(m_onTrue.getRequirements());
    m_requirements.addAll(m_onFalse.getRequirements());
  }

  @Override
  public void initialize() {
    if (m_condition.getAsBoolean()) {
      m_selectedCommand = m_onTrue;
    } else {
      m_selectedCommand = m_onFalse;
    }
    m_selectedCommand.initialize();
  }

  @Override
  public void execute() {
    m_selectedCommand.execute();
  }

  @Override
  public void end(boolean interrupted) {
    m_selectedCommand.end(interrupted);
  }

  @Override
  public boolean isFinished() {
    return m_selectedCommand.isFinished();
  }

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_selectedCommand.runsWhenDisabled();
  }
}

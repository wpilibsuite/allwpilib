package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;
import java.util.function.BooleanSupplier;

public class ConditionalCommand implements Command {

  private final Command m_onTrue;
  private final Command m_onFalse;
  private final BooleanSupplier m_condition;
  private Command m_selectedCommand;

  public ConditionalCommand(Command onTrue, Command onFalse, BooleanSupplier condition) {
    m_onTrue = onTrue;
    m_onFalse = onFalse;
    m_condition = condition;
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
  public void interrupted() {
    m_selectedCommand.interrupted();
  }

  @Override
  public void end() {
    m_selectedCommand.end();
  }

  @Override
  public Set<Subsystem> getRequirements() {
    if (m_condition.getAsBoolean()) {
      return m_onTrue.getRequirements();
    } else {
      return m_onFalse.getRequirements();
    }
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_selectedCommand.runsWhenDisabled();
  }
}

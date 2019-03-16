package edu.wpi.first.wpilibj.experimental.command;

import java.util.HashSet;
import java.util.Set;

public class InstantCommand implements Command{

  private final Runnable m_action;
  private final Set<Subsystem> m_requirements;

  public InstantCommand(Runnable action, Set<Subsystem> requirements) {
    m_action = action;
    m_requirements = requirements;
  }

  public InstantCommand(Runnable action) {
    this(action, new HashSet<>());
  }

  @Override
  public void initialize() {
    m_action.run();
  }

  @Override
  public void execute() {

  }

  @Override
  public void end() {

  }

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public final boolean isFinished() {
    return true;
  }
}

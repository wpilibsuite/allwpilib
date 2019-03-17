package edu.wpi.first.wpilibj.experimental.command;

import java.util.HashSet;
import java.util.Set;

public class InstantCommand extends SendableCommandBase {

  private final Runnable m_action;

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
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public final boolean isFinished() {
    return true;
  }
}

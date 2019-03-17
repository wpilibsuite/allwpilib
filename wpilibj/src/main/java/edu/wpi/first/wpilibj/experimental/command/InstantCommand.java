package edu.wpi.first.wpilibj.experimental.command;

import java.util.HashSet;
import java.util.Set;

/**
 * A Command that runs instantly; it will initialize, execute once, and end on the same
 * iteration of the scheduler.  Users can either pass in a Runnable and a set of requirements,
 * or else subclass this command if desired.
 */
public class InstantCommand extends SendableCommandBase {

  private final Runnable m_action;

  /**
   * Creates a new InstantCommand that performs the given action with the given requirements.
   *
   * @param action the action to perform
   * @param requirements the set of requirements
   */
  public InstantCommand(Runnable action, Set<Subsystem> requirements) {
    m_action = action;
    m_requirements = requirements;
  }

  /**
   * Creates a new InstantCommand that performs the given action with no requirements.
   *
   * @param action the action to perform
   */
  public InstantCommand(Runnable action) {
    this(action, new HashSet<>());
  }

  /**
   * Creates a new InstantCommand with an action that does nothing.  Useful only as a no-arg
   * constructor to call from subclass constructors.
   */
  public InstantCommand() {
    m_action = () -> {};
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

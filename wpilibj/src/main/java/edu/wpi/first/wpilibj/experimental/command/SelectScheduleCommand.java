package edu.wpi.first.wpilibj.experimental.command;

import java.util.Map;
import java.util.function.Supplier;

/**
 * Schedules one of a selection of commands when this command is initialized, depending on the
 * value of the given selector.  Note that if this is run from a CommandGroup, the group will not
 * know about the status of the scheduled command, and will treat this command as finishing
 * instantly.  If this is not desired, consider using {@link SelectCommand}.
 *
 * @param <K> the type of the selector to use, e.g. String or enum
 */
public class SelectScheduleCommand<K> implements Command {

  private final Map<K, Command> m_commands;
  private final Supplier<K> m_selector;
  private final boolean m_interruptible;

  /**
   * Creates a new SelectScheduleCommand.
   *
   * @param commands      the map of commands to choose from
   * @param selector      the selector to determine which command to schedule
   * @param interruptible whether or not the scheduled command should be interruptible
   */
  public SelectScheduleCommand(Map<K, Command> commands, Supplier<K> selector,
                               boolean interruptible) {
    m_commands = commands;
    m_selector = selector;
    m_interruptible = interruptible;
  }

  /**
   * Creates a new SelectScheduleCommand.  The scheduled command will be interruptible.
   *
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to schedule
   */
  public SelectScheduleCommand(Map<K, Command> commands, Supplier<K> selector) {
    this(commands, selector, true);
  }

  @Override
  public void initialize() {
    m_commands.get(m_selector.get()).schedule(m_interruptible);
  }

  @Override
  public void execute() {

  }

  @Override
  public void end() {

  }

  @Override
  public boolean isFinished() {
    return true;
  }
}

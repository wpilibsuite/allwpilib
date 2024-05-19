package edu.wpi.first.wpilibj3.command.async;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.Set;
import java.util.function.Consumer;

@SuppressWarnings("this-escape")
public class RequireableResource {
  private final String name;

  private final AsyncScheduler registeredScheduler;

  private AsyncCommand defaultCommand;

  public RequireableResource(String name) {
    this(name, AsyncScheduler.getInstance());
  }

  public RequireableResource(String name, AsyncScheduler scheduler) {
    this.name = name;
    this.registeredScheduler = scheduler;
    this.defaultCommand = idle();
    scheduler.registerResource(this);
  }

  public String getName() {
    return name;
  }

  /**
   * Sets the default command to run on the resource when no other command is scheduled. The default
   * command's priority is effectively the minimum allowable priority for any command requiring a
   * resource. For this reason, it's required that a default command have a priority less than
   * {@link AsyncCommand#DEFAULT_PRIORITY} to prevent it from blocking other, non-default commands
   * from running.
   *
   * <p>The default command is initially an idle command that merely parks the execution thread.
   * This command has the lowest possible priority so as to allow any other command to run.
   *
   * @param defaultCommand the new default command
   */
  public void setDefaultCommand(AsyncCommand defaultCommand) {
    this.defaultCommand = defaultCommand;
    registeredScheduler.scheduleAsDefaultCommand(this, defaultCommand);
  }

  public AsyncCommand getDefaultCommand() {
    return defaultCommand;
  }

  public AsyncCommandBuilder run(Consumer<Coroutine> command) {
    return new AsyncCommandBuilder().requiring(this).executing(command);
  }

  public AsyncCommand idle() {
    return new IdleCommand(this);
  }

  public AsyncCommand idle(Measure<Time> duration) {
    return idle().withTimeout(duration);
  }

  /**
   * Gets the set of nested resources encapsulated by this one. For compound mechanisms that need to
   * have flexible control over individual components, this will allow the scheduler to permit
   * nested commands to have ownership of just those components and not the entire compound
   * mechanism. In effect, this will allow default commands to run for the components not directly
   * used by that nested command.
   *
   * @return the nested resources
   */
  public Set<RequireableResource> nestedResources() {
    return Set.of();
  }

  @Override
  public String toString() {
    return name;
  }
}

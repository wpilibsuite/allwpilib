// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3;

import edu.wpi.first.units.measure.Time;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.List;
import java.util.function.Consumer;

@SuppressWarnings("this-escape")
public class RequireableResource implements Sendable {
  private final String name;

  private final Scheduler registeredScheduler;

  public RequireableResource(String name) {
    this(name, Scheduler.getInstance());
  }

  public RequireableResource(String name, Scheduler scheduler) {
    this.name = name;
    this.registeredScheduler = scheduler;
    setDefaultCommand(idle());
  }

  public String getName() {
    return name;
  }

  /**
   * Sets the default command to run on the resource when no other command is scheduled. The default
   * command's priority is effectively the minimum allowable priority for any command requiring a
   * resource. For this reason, it's required that a default command have a priority less than
   * {@link Command#DEFAULT_PRIORITY} to prevent it from blocking other, non-default commands from
   * running.
   *
   * <p>The default command is initially an idle command that merely parks the execution thread.
   * This command has the lowest possible priority so as to allow any other command to run.
   *
   * @param defaultCommand the new default command
   */
  public void setDefaultCommand(Command defaultCommand) {
    registeredScheduler.scheduleAsDefaultCommand(this, defaultCommand);
  }

  public Command getDefaultCommand() {
    return registeredScheduler.getDefaultCommandFor(this);
  }

  public CommandBuilder run(Consumer<Coroutine> command) {
    return new CommandBuilder().requiring(this).executing(command);
  }

  public Command idle() {
    return new IdleCommand(this);
  }

  public Command idle(Time duration) {
    return idle().withTimeout(duration);
  }

  public List<Command> getRunningCommands() {
    return registeredScheduler.getRunningCommandsFor(this);
  }

  @Override
  public String toString() {
    return name;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("RequireableResource");
    builder.addStringArrayProperty(
        "Current Commands",
        () -> getRunningCommands().stream().map(Command::name).toArray(String[]::new),
        null);
  }
}

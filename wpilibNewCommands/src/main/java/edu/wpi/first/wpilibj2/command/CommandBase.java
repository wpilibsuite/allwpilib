// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.util.HashSet;
import java.util.Set;

/** A {@link Sendable} base class for {@link Command}s. */
public abstract class CommandBase implements Sendable, Command {
  protected final Set<Subsystem> m_requirements = new HashSet<>();

  protected CommandBase() {
    String name = getClass().getName();
    SendableRegistry.add(this, name.substring(name.lastIndexOf('.') + 1));
  }

  /**
   * Adds the specified requirements to the command.
   *
   * @param requirements the requirements to add
   */
  public final void addRequirements(Subsystem... requirements) {
    m_requirements.addAll(Set.of(requirements));
  }

  @Override
  public final boolean hasRequirement(Subsystem requirement) {
    return m_requirements.contains(requirement);
  }

  @Override
  public final Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public final String getName() {
    return SendableRegistry.getName(this);
  }

  /**
   * Sets the name of this Command.
   *
   * @param name name
   */
  public final void setName(String name) {
    SendableRegistry.setName(this, name);
  }

  /**
   * Decorates this Command with a name. Is an inline function for #setName(String);
   *
   * @param name name
   * @return the decorated Command
   */
  public final CommandBase withName(String name) {
    this.setName(name);
    return this;
  }

  /**
   * Gets the subsystem name of this Command.
   *
   * @return Subsystem name
   */
  public final String getSubsystem() {
    return SendableRegistry.getSubsystem(this);
  }

  /**
   * Sets the subsystem name of this Command.
   *
   * @param subsystem subsystem name
   */
  public final void setSubsystem(String subsystem) {
    SendableRegistry.setSubsystem(this, subsystem);
  }

  /**
   * Initializes this sendable. Useful for allowing implementations to easily extend SendableBase.
   *
   * @param builder the builder used to construct this sendable
   */
  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Command");
    builder.addStringProperty(".name", this::getName, null);
    builder.addBooleanProperty(
        "running",
        this::isScheduled,
        value -> {
          if (value) {
            if (!isScheduled()) {
              schedule();
            }
          } else {
            if (isScheduled()) {
              cancel();
            }
          }
        });
    builder.addBooleanProperty(
        ".isParented", () -> CommandGroupBase.getGroupedCommands().contains(this), null);
  }

  /*
     Stubs to mark utility functions as final.
  */

  @Override
  public final void schedule(boolean interruptible) {
    Command.super.schedule(interruptible);
  }

  @Override
  public final void schedule() {
    Command.super.schedule();
  }

  @Override
  public final void cancel() {
    Command.super.cancel();
  }

  @Override
  public final boolean isScheduled() {
    return Command.super.isScheduled();
  }
}

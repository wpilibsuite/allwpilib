/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.HashSet;
import java.util.Set;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A {@link Sendable} base class for {@link Command}s.
 */
@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class SendableCommandBase implements Sendable, Command {

  protected String m_name = this.getClass().getSimpleName();
  protected String m_subsystem = "Ungrouped";
  protected Set<Subsystem> m_requirements = new HashSet<>();

  /**
   * Adds the specified requirements to the command.
   *
   * @param requirements the requirements to add
   */
  public final void addRequirements(Subsystem... requirements) {
    m_requirements.addAll(Set.of(requirements));
  }

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public String getName() {
    return this.getClass().getSimpleName();
  }

  @Override
  public void setName(String name) {
    m_name = name;
  }

  @Override
  public String getSubsystem() {
    return m_subsystem;
  }

  @Override
  public void setSubsystem(String subsystem) {
    m_subsystem = subsystem;
  }

  /**
   * Initializes this sendable.  Useful for allowing implementations to easily extend SendableBase.
   *
   * @param builder the builder used to construct this sendable
   */
  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Command");
    builder.addStringProperty(".name", this::getName, null);
    builder.addBooleanProperty("running", this::isScheduled, value -> {
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
    builder.addBooleanProperty(".isParented",
        () -> CommandGroupBase.getGroupedCommands().contains(this), null);
  }
}

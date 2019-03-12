/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A base for subsystems that handles registration in the constructor, and provides a more intuitive
 * method for setting the default command.
 */
public abstract class SendableSubsystemBase implements Subsystem, Sendable {

  protected String m_name = this.getClass().getSimpleName();

  public SendableSubsystemBase() {
    CommandScheduler.getInstance().registerSubsystem(this);
  }

  @Override
  public String getName() {
    return m_name;
  }

  @Override
  public void setName(String name) {
    m_name = name;
  }

  @Override
  public String getSubsystem() {
    return getName();
  }

  @Override
  public void setSubsystem(String subsystem) {
    setName(subsystem);
  }

  /**
   * Associates a {@link Sendable} with this Subsystem.
   * Also update the child's name.
   *
   * @param name name to give child
   * @param child sendable
   */
  public void addChild(String name, Sendable child) {
    child.setName(getSubsystem(), name);
    LiveWindow.add(child);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Subsystem");

    builder.addBooleanProperty(".hasDefault", () -> getDefaultCommand() != null, null);
    builder.addStringProperty(".default",
        () -> getDefaultCommand() != null ? getDefaultCommand().getName() : "none", null);
    builder.addBooleanProperty(".hasCommand", () -> getCurrentCommand() != null, null);
    builder.addStringProperty(".command",
        () -> getCurrentCommand() != null ? getCurrentCommand().getName() : "none", null);
  }
}

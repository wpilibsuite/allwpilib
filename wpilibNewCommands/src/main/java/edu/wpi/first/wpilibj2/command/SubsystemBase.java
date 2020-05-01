/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * A base for subsystems that handles registration in the constructor, and provides a more intuitive
 * method for setting the default command.
 */
public abstract class SubsystemBase implements Subsystem, Sendable {

  /**
   * Constructor.
   */
  public SubsystemBase() {
    String name = this.getClass().getSimpleName();
    name = name.substring(name.lastIndexOf('.') + 1);
    SendableRegistry.addLW(this, name, name);
    CommandScheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Gets the name of this Subsystem.
   *
   * @return Name
   */
  @Override
  public String getName() {
    return SendableRegistry.getName(this);
  }

  /**
   * Sets the name of this Subsystem.
   *
   * @param name name
   */
  @Override
  public void setName(String name) {
    SendableRegistry.setName(this, name);
  }

  /**
   * Gets the subsystem name of this Subsystem.
   *
   * @return Subsystem name
   */
  @Override
  public String getSubsystem() {
    return SendableRegistry.getSubsystem(this);
  }

  /**
   * Sets the subsystem name of this Subsystem.
   *
   * @param subsystem subsystem name
   */
  @Override
  public void setSubsystem(String subsystem) {
    SendableRegistry.setSubsystem(this, subsystem);
  }

  /**
   * Associates a {@link Sendable} with this Subsystem.
   * Also update the child's name.
   *
   * @param name name to give child
   * @param child sendable
   */
  public void addChild(String name, Sendable child) {
    SendableRegistry.addLW(child, getSubsystem(), name);
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

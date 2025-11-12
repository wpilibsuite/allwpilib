// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/**
 * A base for subsystems that handles registration in the constructor, and provides a more intuitive
 * method for setting the default command.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public abstract class SubsystemBase implements Subsystem, TelemetryLoggable {
  /** Constructor. Telemetry/log name defaults to the classname. */
  @SuppressWarnings("this-escape")
  public SubsystemBase() {
    String name = this.getClass().getSimpleName();
    m_name = name.substring(name.lastIndexOf('.') + 1);
    CommandScheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Constructor.
   *
   * @param name Name of the subsystem for telemetry and logging.
   */
  @SuppressWarnings("this-escape")
  public SubsystemBase(String name) {
    m_name = name;
    CommandScheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Gets the name of this Subsystem.
   *
   * @return Name
   */
  @Override
  public String getName() {
    return m_name;
  }

  /**
   * Sets the name of this Subsystem.
   *
   * @param name name
   */
  public void setName(String name) {
    m_name = name;
  }

  @Override
  public void updateTelemetry(TelemetryTable table) {
    var defaultCommand = getDefaultCommand();
    table.log(".hasDefault", defaultCommand != null);
    table.log(".default", defaultCommand != null ? defaultCommand.getName() : "none");

    var currentCommand = getCurrentCommand();
    table.log(".hasCommand", currentCommand != null);
    table.log(".command", currentCommand != null ? currentCommand.getName() : "none");
  }

  @Override
  public String getTelemetryType() {
    return "Subsystem";
  }

  private String m_name;
}

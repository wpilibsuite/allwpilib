/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.NamedSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import java.util.Enumeration;
import java.util.Vector;

/**
 * This class defines a major component of the robot.
 *
 * <p>
 * A good example of a subsystem is the driveline, or a claw if the robot has
 * one.
 * </p>
 *
 * <p>
 * All motors should be a part of a subsystem. For instance, all the wheel
 * motors should be a part of some kind of "Driveline" subsystem.
 * </p>
 *
 * <p>
 * Subsystems are used within the command system as requirements for
 * {@link Command}. Only one command which requires a subsystem can run at a
 * time. Also, subsystems can have default commands which are started if there
 * is no command running which requires this subsystem.
 * </p>
 *
 * @author Joe Grinstead
 * @see Command
 */
public abstract class Subsystem implements NamedSendable {

  /** Whether or not getDefaultCommand() was called */
  private boolean initializedDefaultCommand = false;
  /** The current command */
  private Command currentCommand;
  private boolean currentCommandChanged;

  /** The default command */
  private Command defaultCommand;
  /** The name */
  private String name;
  /** List of all subsystems created */
  private static Vector allSubsystems = new Vector();

  /**
   * Creates a subsystem with the given name
   *$
   * @param name the name of the subsystem
   */
  public Subsystem(String name) {
    this.name = name;
    Scheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Creates a subsystem. This will set the name to the name of the class.
   */
  public Subsystem() {
    this.name = getClass().getName().substring(getClass().getName().lastIndexOf('.') + 1);
    Scheduler.getInstance().registerSubsystem(this);
    currentCommandChanged = true;
  }

  /**
   * Initialize the default command for a subsystem By default subsystems have
   * no default command, but if they do, the default command is set with this
   * method. It is called on all Subsystems by CommandBase in the users program
   * after all the Subsystems are created.
   */
  protected abstract void initDefaultCommand();

  /**
   * Sets the default command. If this is not called or is called with null,
   * then there will be no default command for the subsystem.
   *
   * <p>
   * <b>WARNING:</b> This should <b>NOT</b> be called in a constructor if the
   * subsystem is a singleton.
   * </p>
   *
   * @param command the default command (or null if there should be none)
   * @throws IllegalUseOfCommandException if the command does not require the
   *         subsystem
   */
  protected void setDefaultCommand(Command command) {
    if (command == null) {
      defaultCommand = null;
    } else {
      boolean found = false;
      Enumeration requirements = command.getRequirements();
      while (requirements.hasMoreElements()) {
        if (requirements.nextElement().equals(this)) {
          found = true;
          // } else {
          // throw new
          // IllegalUseOfCommandException("A default command cannot require multiple subsystems");
        }
      }
      if (!found) {
        throw new IllegalUseOfCommandException("A default command must require the subsystem");
      }
      defaultCommand = command;
    }
    if (table != null) {
      if (defaultCommand != null) {
        table.putBoolean("hasDefault", true);
        table.putString("default", defaultCommand.getName());
      } else {
        table.putBoolean("hasDefault", false);
      }
    }
  }

  /**
   * Returns the default command (or null if there is none).
   *$
   * @return the default command
   */
  protected Command getDefaultCommand() {
    if (!initializedDefaultCommand) {
      initializedDefaultCommand = true;
      initDefaultCommand();
    }
    return defaultCommand;
  }

  /**
   * Sets the current command
   *$
   * @param command the new current command
   */
  void setCurrentCommand(Command command) {
    currentCommand = command;
    currentCommandChanged = true;
  }

  /**
   * Call this to alert Subsystem that the current command is actually the
   * command. Sometimes, the {@link Subsystem} is told that it has no command
   * while the {@link Scheduler} is going through the loop, only to be soon
   * after given a new one. This will avoid that situation.
   */
  void confirmCommand() {
    if (currentCommandChanged) {
      if (table != null) {
        if (currentCommand != null) {
          table.putBoolean("hasCommand", true);
          table.putString("command", currentCommand.getName());
        } else {
          table.putBoolean("hasCommand", false);
        }
      }
      currentCommandChanged = false;
    }
  }

  /**
   * Returns the command which currently claims this subsystem.
   *$
   * @return the command which currently claims this subsystem
   */
  public Command getCurrentCommand() {
    return currentCommand;
  }

  public String toString() {
    return getName();
  }

  /**
   * Returns the name of this subsystem, which is by default the class name.
   *$
   * @return the name of this subsystem
   */
  public String getName() {
    return name;
  }

  public String getSmartDashboardType() {
    return "Subsystem";
  }

  private ITable table;

  public void initTable(ITable table) {
    this.table = table;
    if (table != null) {
      if (defaultCommand != null) {
        table.putBoolean("hasDefault", true);
        table.putString("default", defaultCommand.getName());
      } else {
        table.putBoolean("hasDefault", false);
      }
      if (currentCommand != null) {
        table.putBoolean("hasCommand", true);
        table.putString("command", currentCommand.getName());
      } else {
        table.putBoolean("hasCommand", false);
      }
    }
  }

  /**
   * {@inheritDoc}
   */
  public ITable getTable() {
    return table;
  }
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import java.util.Enumeration;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.NamedSendable;

/**
 * This class defines a major component of the robot.
 *
 * <p> A good example of a subsystem is the driveline, or a claw if the robot has one. </p>
 *
 * <p> All motors should be a part of a subsystem. For instance, all the wheel motors should be a
 * part of some kind of "Driveline" subsystem. </p>
 *
 * <p> Subsystems are used within the command system as requirements for {@link Command}. Only one
 * command which requires a subsystem can run at a time. Also, subsystems can have default commands
 * which are started if there is no command running which requires this subsystem. </p>
 *
 * @see Command
 */
public abstract class Subsystem implements NamedSendable {

  /**
   * Whether or not getDefaultCommand() was called.
   */
  private boolean m_initializedDefaultCommand = false;
  /**
   * The current command.
   */
  private Command m_currentCommand;
  private boolean m_currentCommandChanged;

  /**
   * The default command.
   */
  private Command m_defaultCommand;
  /**
   * The name.
   */
  private String m_name;

  /**
   * Creates a subsystem with the given name.
   *
   * @param name the name of the subsystem
   */
  public Subsystem(String name) {
    m_name = name;
    Scheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Creates a subsystem. This will set the name to the name of the class.
   */
  public Subsystem() {
    m_name = getClass().getName().substring(getClass().getName().lastIndexOf('.') + 1);
    Scheduler.getInstance().registerSubsystem(this);
    m_currentCommandChanged = true;
  }

  /**
   * Initialize the default command for a subsystem By default subsystems have no default command,
   * but if they do, the default command is set with this method. It is called on all Subsystems by
   * CommandBase in the users program after all the Subsystems are created.
   */
  protected abstract void initDefaultCommand();

  /**
   * When the run method of the scheduler is called this method will be called.
   */
  public void periodic() {
    // Override me!
  }

  /**
   * Sets the default command. If this is not called or is called with null, then there will be no
   * default command for the subsystem.
   *
   * <p> <b>WARNING:</b> This should <b>NOT</b> be called in a constructor if the subsystem is a
   * singleton. </p>
   *
   * @param command the default command (or null if there should be none)
   * @throws IllegalUseOfCommandException if the command does not require the subsystem
   */
  protected void setDefaultCommand(Command command) {
    if (command == null) {
      m_defaultCommand = null;
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
      m_defaultCommand = command;
    }
    if (m_hasDefaultEntry != null && m_defaultEntry != null) {
      if (m_defaultCommand != null) {
        m_hasDefaultEntry.setBoolean(true);
        m_defaultEntry.setString(m_defaultCommand.getName());
      } else {
        m_hasDefaultEntry.setBoolean(false);
      }
    }
  }

  /**
   * Returns the default command (or null if there is none).
   *
   * @return the default command
   */
  protected Command getDefaultCommand() {
    if (!m_initializedDefaultCommand) {
      m_initializedDefaultCommand = true;
      initDefaultCommand();
    }
    return m_defaultCommand;
  }

  /**
   * Sets the current command.
   *
   * @param command the new current command
   */
  void setCurrentCommand(Command command) {
    m_currentCommand = command;
    m_currentCommandChanged = true;
  }

  /**
   * Call this to alert Subsystem that the current command is actually the command. Sometimes, the
   * {@link Subsystem} is told that it has no command while the {@link Scheduler} is going through
   * the loop, only to be soon after given a new one. This will avoid that situation.
   */
  void confirmCommand() {
    if (m_currentCommandChanged) {
      if (m_hasCommandEntry != null && m_commandEntry != null) {
        if (m_currentCommand != null) {
          m_hasCommandEntry.setBoolean(true);
          m_commandEntry.setString(m_currentCommand.getName());
        } else {
          m_hasCommandEntry.setBoolean(false);
        }
      }
      m_currentCommandChanged = false;
    }
  }

  /**
   * Returns the command which currently claims this subsystem.
   *
   * @return the command which currently claims this subsystem
   */
  public Command getCurrentCommand() {
    return m_currentCommand;
  }

  @Override
  public String toString() {
    return getName();
  }

  /**
   * Returns the name of this subsystem, which is by default the class name.
   *
   * @return the name of this subsystem
   */
  @Override
  public String getName() {
    return m_name;
  }

  @Override
  public String getSmartDashboardType() {
    return "Subsystem";
  }

  private NetworkTableEntry m_hasDefaultEntry;
  private NetworkTableEntry m_defaultEntry;
  private NetworkTableEntry m_hasCommandEntry;
  private NetworkTableEntry m_commandEntry;

  @Override
  public void initTable(NetworkTable table) {
    if (table != null) {
      m_hasDefaultEntry = table.getEntry("hasDefault");
      m_defaultEntry = table.getEntry("default");
      m_hasCommandEntry = table.getEntry("hasCommand");
      m_commandEntry = table.getEntry("command");

      if (m_defaultCommand != null) {
        m_hasDefaultEntry.setBoolean(true);
        m_defaultEntry.setString(m_defaultCommand.getName());
      } else {
        m_hasDefaultEntry.setBoolean(false);
      }
      if (m_currentCommand != null) {
        m_hasCommandEntry.setBoolean(true);
        m_commandEntry.setString(m_currentCommand.getName());
      } else {
        m_hasCommandEntry.setBoolean(false);
      }
    }
  }
}

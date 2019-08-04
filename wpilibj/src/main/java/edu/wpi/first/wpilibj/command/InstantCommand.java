/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * This command will execute once, then finish immediately afterward.
 *
 * <p>Subclassing {@link InstantCommand} is shorthand for returning true from
 * {@link Command isFinished}.
 */
public class InstantCommand extends Command {
  private Runnable m_func;

  public InstantCommand() {
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand} with the given name.
   *
   * @param name the name for this command
   */
  public InstantCommand(String name) {
    super(name);
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand} with the given requirement.
   *
   * @param subsystem the subsystem this command requires
   */
  public InstantCommand(Subsystem subsystem) {
    super(subsystem);
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand} with the given name and requirement.
   *
   * @param name      the name for this command
   * @param subsystem the subsystem this command requires
   */
  public InstantCommand(String name, Subsystem subsystem) {
    super(name, subsystem);
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand}.
   *
   * @param func the function to run when {@link Command#initialize() initialize()} is run
   */
  public InstantCommand(Runnable func) {
    m_func = func;
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand}.
   *
   * @param name the name for this command
   * @param func the function to run when {@link Command#initialize() initialize()} is run
   */
  public InstantCommand(String name, Runnable func) {
    super(name);
    m_func = func;
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand}.
   *
   * @param requirement the subsystem this command requires
   * @param func        the function to run when {@link Command#initialize() initialize()} is run
   */
  public InstantCommand(Subsystem requirement, Runnable func) {
    super(requirement);
    m_func = func;
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand}.
   *
   * @param name        the name for this command
   * @param requirement the subsystem this command requires
   * @param func        the function to run when {@link Command#initialize() initialize()} is run
   */
  public InstantCommand(String name, Subsystem requirement, Runnable func) {
    super(name, requirement);
    m_func = func;
  }

  @Override
  protected boolean isFinished() {
    return true;
  }

  /**
   * Trigger the stored function.
   *
   * <p>Called just before this Command runs the first time.
   */
  @Override
  protected void _initialize() {
    super._initialize();
    if (m_func != null) {
      m_func.run();
    }
  }
}

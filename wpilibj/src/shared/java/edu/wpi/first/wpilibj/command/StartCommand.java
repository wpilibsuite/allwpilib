/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * A {@link StartCommand} will call the {@link Command#start() start()} method
 * of another command when it is initialized and will finish immediately.
 *
 * @author Joe Grinstead
 */
public class StartCommand extends Command {

  /** The command to fork */
  private Command m_commandToFork;

  /**
   * Instantiates a {@link StartCommand} which will start the given command
   * whenever its {@link Command#initialize() initialize()} is called.
   *$
   * @param commandToStart the {@link Command} to start
   */
  public StartCommand(Command commandToStart) {
    super("Start(" + commandToStart + ")");
    m_commandToFork = commandToStart;
  }

  protected void initialize() {
    m_commandToFork.start();
  }

  protected void execute() {}

  protected boolean isFinished() {
    return true;
  }

  protected void end() {}

  protected void interrupted() {}
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

/**
 * A {@link StartCommand} will call the {@link Command#start() start()} method of another command
 * when it is initialized and will finish immediately.
 */
public class StartCommand extends InstantCommand {
  /**
   * The command to fork.
   */
  private final Command m_commandToFork;

  /**
   * Instantiates a {@link StartCommand} which will start the given command whenever its {@link
   * Command#initialize() initialize()} is called.
   *
   * @param commandToStart the {@link Command} to start
   */
  public StartCommand(Command commandToStart) {
    super("Start(" + commandToStart + ")");
    m_commandToFork = commandToStart;
  }

  @Override
  protected void initialize() {
    m_commandToFork.start();
  }
}

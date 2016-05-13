/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.mocks.MockCommand;

/**
 * Ported from the old CrioTest Classes.
 *
 * @author Mitchell
 * @author Jonathan Leitschuh
 */
public class CommandScheduleTest extends AbstractCommandTest {
  private static final Logger logger = Logger.getLogger(CommandScheduleTest.class.getName());

  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Simple scheduling of a command and making sure the command is run and successfully terminates.
   */
  @Test
  public void testRunAndTerminate() {
    final MockCommand command = new MockCommand();
    command.start();
    assertCommandState(command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 2, 2, 0, 0);
    command.setHasFinished(true);
    assertCommandState(command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 3, 3, 1, 0);
  }

  /**
   * Simple scheduling of a command and making sure the command is run and cancels correctly.
   */
  @Test
  public void testRunAndCancel() {
    final MockCommand command = new MockCommand();
    command.start();
    assertCommandState(command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 3, 3, 0, 0);
    command.cancel();
    assertCommandState(command, 1, 3, 3, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 3, 3, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 3, 3, 0, 1);
  }
}

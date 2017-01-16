/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.Test;

import java.util.logging.Logger;

/**
 * Ported from the old CrioTest Classes.
 */
public class CommandSequentialGroupTest extends AbstractCommandTest {
  private static Logger logger = Logger.getLogger(CommandSequentialGroupTest.class.getName());

  /**
   * Simple Command Group With 3 commands that all depend on a subsystem. Some commands have a
   * timeout.
   */
  @Test(timeout = 20000)
  public void testThreeCommandOnSubSystem() {
    logger.fine("Begining Test");
    final ASubsystem subsystem = new ASubsystem();

    logger.finest("Creating Mock Command1");
    final MockCommand command1 = new MockCommand() {
      {
        requires(subsystem);
      }
    };
    logger.finest("Creating Mock Command2");
    final MockCommand command2 = new MockCommand() {
      {
        requires(subsystem);
      }
    };
    logger.finest("Creating Mock Command3");
    final MockCommand command3 = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    logger.finest("Creating Command Group");
    final CommandGroup commandGroup = new CommandGroup();
    commandGroup.addSequential(command1, 1.0);
    commandGroup.addSequential(command2, 2.0);
    commandGroup.addSequential(command3);


    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    logger.finest("Starting Command group");
    commandGroup.start();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    sleep(1000); // command 1 timeout
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 1, 1, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);

    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    sleep(2000); // command 2 timeout
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 1);
    assertCommandState(command3, 1, 1, 1, 0, 0);

    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 1);
    assertCommandState(command3, 1, 2, 2, 0, 0);
    command3.setHasFinished(true);
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 1);
    assertCommandState(command3, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 1);
    assertCommandState(command3, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 1);
    assertCommandState(command3, 1, 3, 3, 1, 0);
  }

}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.Test;

/**
 * Tests the {@link Command} library.
 */
public class DefaultCommandTest extends AbstractCommandTest {


  /**
   * Testing of default commands where the interrupting command ends itself.
   */
  @Test
  public void testDefaultCommandWhereTheInteruptingCommandEndsItself() {
    final ASubsystem subsystem = new ASubsystem();


    final MockCommand defaultCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    final MockCommand anotherCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    subsystem.init(defaultCommand);

    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);

    anotherCommand.start();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    anotherCommand.setHasFinished(true);
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 4, 4, 0, 1);
    assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 5, 5, 0, 1);
    assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
  }


  /**
   * Testing of default commands where the interrupting command is canceled.
   */
  @Test
  public void testDefaultCommandsInterruptingCommandCanceled() {
    final ASubsystem subsystem = new ASubsystem();


    final MockCommand defaultCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    final MockCommand anotherCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    subsystem.init(defaultCommand);
    subsystem.initDefaultCommand();
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);

    anotherCommand.start();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    anotherCommand.cancel();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 4, 4, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 5, 5, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
  }

}

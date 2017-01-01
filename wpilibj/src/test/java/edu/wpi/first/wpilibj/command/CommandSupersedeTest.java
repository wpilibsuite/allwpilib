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
public class CommandSupersedeTest extends AbstractCommandTest {
  private static final Logger logger = Logger.getLogger(CommandSupersedeTest.class.getName());


  /**
   * Testing one command superseding another because of dependencies.
   */
  @Test
  public void testOneCommandSupersedingAnotherBecauseOfDependencies() {
    final ASubsystem subsystem = new ASubsystem();

    final MockCommand command1 = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    final MockCommand command2 = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    command1.start();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 2, 2, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 3, 3, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    command2.start();
    assertCommandState(command1, 1, 3, 3, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 4, 4, 0, 1);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 4, 4, 0, 1);
    assertCommandState(command2, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 4, 4, 0, 1);
    assertCommandState(command2, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 4, 4, 0, 1);
    assertCommandState(command2, 1, 3, 3, 0, 0);
  }

  /**
   * Testing one command failing superseding another because of dependencies because the first
   * command cannot be interrupted.
   */
  @Test
  public void testCommandFailingSupersedingBecauseFirstCanNotBeInterrupted() {
    final ASubsystem subsystem = new ASubsystem();

    final MockCommand command1 = new MockCommand() {
      {
        requires(subsystem);
        setInterruptible(false);
      }
    };

    final MockCommand command2 = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    command1.start();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 2, 2, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 3, 3, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    command2.start();
    assertCommandState(command1, 1, 3, 3, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 4, 4, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
  }

}

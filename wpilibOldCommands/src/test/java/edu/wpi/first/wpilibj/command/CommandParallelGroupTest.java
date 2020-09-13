/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

/**
 * Ported from the old CrioTest Classes.
 */
class CommandParallelGroupTest extends AbstractCommandTest {
  /**
   * Simple Parallel Command Group With 2 commands one command terminates first.
   */
  @Test
  void parallelCommandGroupWithTwoCommandsTest() {
    final MockCommand command1 = new MockCommand();
    final MockCommand command2 = new MockCommand();

    try (CommandGroup commandGroup = new CommandGroup()) {
      commandGroup.addParallel(command1);
      commandGroup.addParallel(command2);

      assertCommandState(command1, 0, 0, 0, 0, 0);
      assertCommandState(command2, 0, 0, 0, 0, 0);
      commandGroup.start();
      assertCommandState(command1, 0, 0, 0, 0, 0);
      assertCommandState(command2, 0, 0, 0, 0, 0);
      Scheduler.getInstance().run();
      assertCommandState(command1, 0, 0, 0, 0, 0);
      assertCommandState(command2, 0, 0, 0, 0, 0);
      Scheduler.getInstance().run();
      assertCommandState(command1, 1, 1, 1, 0, 0);
      assertCommandState(command2, 1, 1, 1, 0, 0);
      Scheduler.getInstance().run();
      assertCommandState(command1, 1, 2, 2, 0, 0);
      assertCommandState(command2, 1, 2, 2, 0, 0);
      command1.setHasFinished(true);
      Scheduler.getInstance().run();
      assertCommandState(command1, 1, 3, 3, 1, 0);
      assertCommandState(command2, 1, 3, 3, 0, 0);
      Scheduler.getInstance().run();
      assertCommandState(command1, 1, 3, 3, 1, 0);
      assertCommandState(command2, 1, 4, 4, 0, 0);
      command2.setHasFinished(true);
      Scheduler.getInstance().run();
      assertCommandState(command1, 1, 3, 3, 1, 0);
      assertCommandState(command2, 1, 5, 5, 1, 0);
    }
  }

}

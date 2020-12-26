// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

/**
 * Ported from the old CrioTest Classes.
 */
class CommandSupersedeTest extends AbstractCommandTest {
  /**
   * Testing one command superseding another because of dependencies.
   */
  @Test
  void oneCommandSupersedingAnotherBecauseOfDependenciesTest() {
    final ASubsystem subsystem = new ASubsystem();

    final MockCommand command1 = new MockCommand(subsystem);

    final MockCommand command2 = new MockCommand(subsystem);

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
  @SuppressWarnings("PMD.NonStaticInitializer")
  void commandFailingSupersedingBecauseFirstCanNotBeInterruptedTest() {
    final ASubsystem subsystem = new ASubsystem();

    final MockCommand command1 = new MockCommand(subsystem) {
      {
        setInterruptible(false);
      }
    };

    final MockCommand command2 = new MockCommand(subsystem);

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

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

/** Ported from the old CrioTest Classes. */
class CommandScheduleTest extends AbstractCommandTest {
  /**
   * Simple scheduling of a command and making sure the command is run and successfully terminates.
   */
  @Test
  void runAndTerminateTest() {
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

  /** Simple scheduling of a command and making sure the command is run and cancels correctly. */
  @Test
  void runAndCancelTest() {
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

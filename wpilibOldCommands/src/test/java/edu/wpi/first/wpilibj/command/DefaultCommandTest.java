// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

/** Tests the {@link Command} library. */
class DefaultCommandTest extends AbstractCommandTest {
  /** Testing of default commands where the interrupting command ends itself. */
  @Test
  void defaultCommandWhereTheInteruptingCommandEndsItselfTest() {
    final ASubsystem subsystem = new ASubsystem();

    final MockCommand defaultCommand = new MockCommand(subsystem);

    final MockCommand anotherCommand = new MockCommand(subsystem);
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

  /** Testing of default commands where the interrupting command is canceled. */
  @Test
  void defaultCommandsInterruptingCommandCanceledTest() {
    final ASubsystem subsystem = new ASubsystem();
    final MockCommand defaultCommand = new MockCommand(subsystem);
    final MockCommand anotherCommand = new MockCommand(subsystem);

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

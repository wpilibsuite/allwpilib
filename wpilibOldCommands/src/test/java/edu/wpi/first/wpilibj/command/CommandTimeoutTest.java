// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

/** Test a {@link Command} that times out. */
class CommandTimeoutTest extends AbstractCommandTest {
  /** Command 2 second Timeout Test. */
  @Test
  void twoSecondTimeoutTest() {
    final ASubsystem subsystem = new ASubsystem();

    final MockCommand command =
        new MockCommand(subsystem, 2) {
          @Override
          public boolean isFinished() {
            return super.isFinished() || isTimedOut();
          }
        };

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
    sleep(2500);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 4, 4, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 4, 4, 1, 0);
  }
}

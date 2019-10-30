/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

/**
 * Test a {@link Command} that times out.
 */
class CommandTimeoutTest extends AbstractCommandTest {
  /**
   * Command 2 second Timeout Test.
   */
  @Test
  void twoSecondTimeoutTest() {
    final ASubsystem subsystem = new ASubsystem();


    final MockCommand command = new MockCommand(subsystem, 2) {
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

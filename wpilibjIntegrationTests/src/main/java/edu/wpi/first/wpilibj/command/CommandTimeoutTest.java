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
 * Test a {@link Command} that times out.
 *
 * @author jonathanleitschuh
 */
public class CommandTimeoutTest extends AbstractCommandTest {
  private static final Logger logger = Logger.getLogger(CommandTimeoutTest.class.getName());

  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Command 2 second Timeout Test.
   */
  @Test
  public void testTwoSecondTimeout() {
    final ASubsystem subsystem = new ASubsystem();


    final MockCommand command = new MockCommand() {
      {
        requires(subsystem);
        setTimeout(2);
      }

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
    sleep(2000);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 4, 4, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 4, 4, 1, 0);
  }

}

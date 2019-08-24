/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.Timer;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.anyDouble;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

class WaitCommandTest extends CommandTestBase {
  @Test
  void waitCommandTest() {
    CommandScheduler scheduler = new CommandScheduler();

    WaitCommand waitCommand = new WaitCommand(2);

    scheduler.schedule(waitCommand);
    scheduler.run();
    Timer.delay(1);
    scheduler.run();

    assertTrue(scheduler.isScheduled(waitCommand));

    Timer.delay(2);

    scheduler.run();

    assertFalse(scheduler.isScheduled(waitCommand));
  }

  @Test
  void withTimeoutTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    when(command1.withTimeout(anyDouble())).thenCallRealMethod();

    Command timeout = command1.withTimeout(2);

    scheduler.schedule(timeout);
    scheduler.run();

    verify(command1).initialize();
    verify(command1).execute();
    assertFalse(scheduler.isScheduled(command1));
    assertTrue(scheduler.isScheduled(timeout));

    Timer.delay(3);
    scheduler.run();

    verify(command1).end(true);
    verify(command1, never()).end(false);
    assertFalse(scheduler.isScheduled(timeout));
  }
}

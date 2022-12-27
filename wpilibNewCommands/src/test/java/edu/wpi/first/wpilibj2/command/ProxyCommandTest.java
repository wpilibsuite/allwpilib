// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.verify;

import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;

class ProxyCommandTest extends CommandTestBase {
  @Test
  void proxyCommandScheduleTest() {
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();

    ProxyCommand scheduleCommand = new ProxyCommand(command1);

    scheduleCommand.schedule();

    verify(command1).schedule();
  }

  @Test
  void proxyCommandEndTest() {
    AtomicBoolean cond = new AtomicBoolean();

    WaitUntilCommand command = new WaitUntilCommand(cond::get);

    ProxyCommand scheduleCommand = new ProxyCommand(command);

    scheduleCommand.schedule();

    CommandScheduler.getInstance().run();
    assertTrue(scheduleCommand.isScheduled());

    cond.set(true);
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    assertFalse(scheduleCommand.isScheduled());
  }
}

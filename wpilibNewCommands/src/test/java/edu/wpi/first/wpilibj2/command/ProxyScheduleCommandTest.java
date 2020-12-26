// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.verify;

class ProxyScheduleCommandTest extends CommandTestBase {
  @Test
  void proxyScheduleCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();

      ProxyScheduleCommand scheduleCommand = new ProxyScheduleCommand(command1);

      scheduler.schedule(scheduleCommand);

      verify(command1).schedule();
    }
  }

  @Test
  void proxyScheduleCommandEndTest() {
    try (CommandScheduler scheduler = CommandScheduler.getInstance()) {
      ConditionHolder cond = new ConditionHolder();

      WaitUntilCommand command = new WaitUntilCommand(cond::getCondition);

      ProxyScheduleCommand scheduleCommand = new ProxyScheduleCommand(command);

      scheduler.schedule(scheduleCommand);

      scheduler.run();
      assertTrue(scheduler.isScheduled(scheduleCommand));

      cond.setCondition(true);
      scheduler.run();
      scheduler.run();
      assertFalse(scheduler.isScheduled(scheduleCommand));
    }
  }
}

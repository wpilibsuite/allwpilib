/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

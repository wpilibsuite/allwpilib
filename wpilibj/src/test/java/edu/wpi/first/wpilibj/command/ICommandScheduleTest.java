package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.mockito.Mockito.verify;


class ICommandScheduleTest extends ICommandTestBase{

  @Test
  void instantScheduleTest() {
    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    holder.setFinished(true);
    ICommand mockCommand = holder.getMock();

    SchedulerNew.getInstance().scheduleCommand(mockCommand, true);
    SchedulerNew.getInstance().run();

    verify(mockCommand).initialize();
    verify(mockCommand).execute();
    verify(mockCommand).end();

    assertFalse(SchedulerNew.getInstance().isScheduled(mockCommand));
  }
}

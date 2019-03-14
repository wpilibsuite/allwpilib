package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class CommandGroupErrorTest extends ICommandTestBase {
  @Test
  void commandInMultipleGroupsTest() {
    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command2 = command2Holder.getMock();

    ICommand group = new ParallelCommandGroup(command1, command2);
    assertThrows(IllegalUseOfCommandException.class, () -> new ParallelCommandGroup(command1, command2));
  }

  @Test
  void commandInGroupExternallyScheduledTest() {
    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command2 = command2Holder.getMock();

    ICommand group = new ParallelCommandGroup(command1, command2);
    assertThrows(IllegalUseOfCommandException.class, () -> scheduler.scheduleCommand(command1, true));
  }


}

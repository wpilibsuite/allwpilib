package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class CommandGroupErrorTest extends CommandTestBase {
  @Test
  void commandInMultipleGroupsTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new ParallelCommandGroup(command1, command2);
    assertThrows(IllegalUseOfCommandException.class, () -> new ParallelCommandGroup(command1, command2));
  }

  @Test
  void commandInGroupExternallyScheduledTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new ParallelCommandGroup(command1, command2);
    assertThrows(IllegalUseOfCommandException.class, () -> scheduler.scheduleCommand(command1, true));
  }


}

package edu.wpi.first.wpilibj.experimental.command;

import java.util.Map;

import org.junit.jupiter.api.Test;

import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

public class SelectCommandTest extends CommandTestBase {
  @Test
  void selectCommandTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    command1Holder.setFinished(true);
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();

    SelectCommand selectCommand =
        new SelectCommand(Map.ofEntries(
            Map.entry("one", command1),
            Map.entry("two", command2),
            Map.entry("three", command3)),
            () -> "one");

    scheduler.scheduleCommand(selectCommand, true);
    scheduler.run();

    verify(command1).initialize();
    verify(command1).execute();
    verify(command1).end(false);

    verify(command2, never()).initialize();
    verify(command2, never()).execute();
    verify(command2, never()).end(false);

    verify(command3, never()).initialize();
    verify(command3, never()).execute();
    verify(command3, never()).end(false);
  }

  @Test
  void conditionalCommandTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    command1Holder.setFinished(true);
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();

    ConditionalCommand conditionalCommand = new ConditionalCommand(command1, command2, () -> true);

    scheduler.scheduleCommand(conditionalCommand, true);
    scheduler.run();

    verify(command1).initialize();
    verify(command1).execute();
    verify(command1).end(false);

    verify(command2, never()).initialize();
    verify(command2, never()).execute();
    verify(command2, never()).end(false);
  }
}

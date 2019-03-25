package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.Timer;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.anyDouble;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class WaitCommandTest extends CommandTestBase {
  @Test
  void waitCommandTest() {
    CommandScheduler scheduler = new CommandScheduler();

    WaitCommand waitCommand = new WaitCommand(2);

    scheduler.scheduleCommand(waitCommand, true);
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

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    when(command1.withTimeout(anyDouble())).thenCallRealMethod();

    Command timeout = command1.withTimeout(2);

    scheduler.scheduleCommand(timeout, true);
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

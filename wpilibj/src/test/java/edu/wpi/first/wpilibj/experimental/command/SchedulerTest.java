package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

class SchedulerTest extends CommandTestBase {
  @Test
  void schedulerLambdaTestNoInterrupt() {
    CommandScheduler scheduler = new CommandScheduler();

    Counter counter = new Counter();

    scheduler.onCommandInitialize(command -> counter.increment());
    scheduler.onCommandExecute(command -> counter.increment());
    scheduler.onCommandFinished(command -> counter.increment());

    scheduler.scheduleCommand(new InstantCommand(), true);
    scheduler.run();

    assertEquals(counter.m_counter, 3);
  }

  @Test
  void schedulerInterruptLambdaTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Counter counter = new Counter();

    scheduler.onCommandInterrupted(command -> counter.increment());

    Command command = new WaitCommand(10);

    scheduler.scheduleCommand(command, true);
    scheduler.cancelCommand(command);

    assertEquals(counter.m_counter, 1);
  }

  @Test
  void unregisterSubsystemTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Subsystem system = new TestSubsystem();

    scheduler.registerSubsystem(system);
    assertDoesNotThrow(() -> scheduler.unregisterSubsystem(system));
  }
}

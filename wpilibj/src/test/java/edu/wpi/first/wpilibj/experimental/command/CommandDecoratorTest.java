package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.Timer;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class CommandDecoratorTest extends CommandTestBase {
  @Test
  void withTimeoutTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Command command1 = new WaitCommand(10);

    Command timeout = command1.withTimeout(2);

    scheduler.scheduleCommand(timeout, true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(command1));
    assertTrue(scheduler.isScheduled(timeout));

    Timer.delay(3);
    scheduler.run();

    assertFalse(scheduler.isScheduled(timeout));
  }

  @Test
  void interruptOnTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();

    Command command = new WaitCommand(10).interruptOn(condition::getCondition);

    scheduler.scheduleCommand(command, true);
    scheduler.run();
    assertTrue(scheduler.isScheduled(command));
    condition.setCondition(true);
    scheduler.run();
    assertFalse(scheduler.isScheduled(command));
  }

  @Test
  void beforeStartingTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();
    condition.setCondition(false);

    Command command = new InstantCommand();

    scheduler.scheduleCommand(command.beforeStarting(() -> condition.setCondition(true)), true);

    assertTrue(condition.getCondition());
  }

  @Test
  void whenFinishedTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();
    condition.setCondition(false);

    Command command = new InstantCommand();

    scheduler.scheduleCommand(command.whenFinished(() -> condition.setCondition(true)), true);

    assertFalse(condition.getCondition());

    scheduler.run();

    assertTrue(condition.getCondition());
  }

  @Test
  void andThenTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();
    condition.setCondition(false);

    Command command1 = new InstantCommand();
    Command command2 = new InstantCommand(() -> condition.setCondition(true));

    scheduler.scheduleCommand(command1.andThen(command2), true);

    assertFalse(condition.getCondition());

    scheduler.run();

    assertTrue(condition.getCondition());
  }

  @Test
  void dictatingTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();
    condition.setCondition(false);

    Command dictator = new WaitUntilCommand(condition::getCondition);
    Command endsBefore = new InstantCommand();
    Command endsAfter = new WaitUntilCommand(() -> false);

    Command group = dictator.dictating(endsBefore, endsAfter);

    scheduler.scheduleCommand(group, true);
    scheduler.run();

    assertTrue(scheduler.isScheduled(group));

    condition.setCondition(true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void alongWithTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();
    condition.setCondition(false);

    Command command1 = new WaitUntilCommand(condition::getCondition);
    Command command2 = new InstantCommand();

    Command group = command1.alongWith(command2);

    scheduler.scheduleCommand(group, true);
    scheduler.run();

    assertTrue(scheduler.isScheduled(group));

    condition.setCondition(true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void raceWithTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Command command1 = new WaitUntilCommand(() -> false);
    Command command2 = new InstantCommand();

    Command group = command1.raceWith(command2);

    scheduler.scheduleCommand(group, true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void perpetuallyTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Command command = new InstantCommand();

    Command perpetual = command.perpetually();

    scheduler.scheduleCommand(perpetual, true);
    scheduler.run();
    scheduler.run();
    scheduler.run();

    assertTrue(scheduler.isScheduled(perpetual));
  }
}

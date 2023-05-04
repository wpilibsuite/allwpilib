// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class CommandDecoratorTest extends CommandTestBase {
  @Test
  @ResourceLock("timing")
  void withTimeoutTest() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    try {
      Command command1 = new WaitCommand(10);

      Command timeout = command1.withTimeout(2);

      timeout.schedule();
      CommandScheduler.getInstance().run();

      assertFalse(command1.isScheduled());
      assertTrue(timeout.isScheduled());

      SimHooks.stepTiming(3);
      CommandScheduler.getInstance().run();

      assertFalse(timeout.isScheduled());
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @Test
  void untilTest() {
    AtomicBoolean condition = new AtomicBoolean();

    Command command = new WaitCommand(10).until(condition::get);

    command.schedule();
    CommandScheduler.getInstance().run();
    assertTrue(command.isScheduled());
    condition.set(true);
    CommandScheduler.getInstance().run();
    assertFalse(command.isScheduled());
  }

  @Test
  void onlyWhileTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean(true);

      Command command = new WaitCommand(10).onlyWhile(condition::get);

      scheduler.schedule(command);
      scheduler.run();
      assertTrue(scheduler.isScheduled(command));
      condition.set(false);
      scheduler.run();
      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void ignoringDisableTest() {
    var command = new RunCommand(() -> {}).ignoringDisable(true);

    setDSEnabled(false);

    command.schedule();

    CommandScheduler.getInstance().run();
    assertTrue(command.isScheduled());
  }

  @Test
  void beforeStartingTest() {
    AtomicBoolean condition = new AtomicBoolean();
    condition.set(false);

    Command command = new InstantCommand();

    command.beforeStarting(() -> condition.set(true)).schedule();

    assertTrue(condition.get());
  }

  @Test
  void andThenLambdaTest() {
    AtomicBoolean condition = new AtomicBoolean();
    condition.set(false);

    Command command = new InstantCommand();

    command.andThen(() -> condition.set(true)).schedule();

    assertFalse(condition.get());

    CommandScheduler.getInstance().run();

    assertTrue(condition.get());
  }

  @Test
  void andThenTest() {
    AtomicBoolean condition = new AtomicBoolean();
    condition.set(false);

    Command command1 = new InstantCommand();
    Command command2 = new InstantCommand(() -> condition.set(true));

    command1.andThen(command2).schedule();

    assertFalse(condition.get());

    CommandScheduler.getInstance().run();

    assertTrue(condition.get());
  }

  @Test
  void deadlineWithTest() {
    AtomicBoolean condition = new AtomicBoolean();
    condition.set(false);

    Command dictator = new WaitUntilCommand(condition::get);
    Command endsBefore = new InstantCommand();
    Command endsAfter = new WaitUntilCommand(() -> false);

    Command group = dictator.deadlineWith(endsBefore, endsAfter);

    group.schedule();
    CommandScheduler.getInstance().run();

    assertTrue(group.isScheduled());

    condition.set(true);
    CommandScheduler.getInstance().run();

    assertFalse(group.isScheduled());
  }

  @Test
  void alongWithTest() {
    AtomicBoolean condition = new AtomicBoolean();
    condition.set(false);

    Command command1 = new WaitUntilCommand(condition::get);
    Command command2 = new InstantCommand();

    Command group = command1.alongWith(command2);

    group.schedule();
    CommandScheduler.getInstance().run();

    assertTrue(group.isScheduled());

    condition.set(true);
    CommandScheduler.getInstance().run();

    assertFalse(group.isScheduled());
  }

  @Test
  void raceWithTest() {
    Command command1 = new WaitUntilCommand(() -> false);
    Command command2 = new InstantCommand();

    Command group = command1.raceWith(command2);

    group.schedule();
    CommandScheduler.getInstance().run();

    assertFalse(group.isScheduled());
  }

  @SuppressWarnings("removal") // Command.perpetually()
  @Test
  void perpetuallyTest() {
    Command command = new InstantCommand();

    Command perpetual = command.perpetually();

    perpetual.schedule();
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();

    assertTrue(perpetual.isScheduled());
  }

  @Test
  void unlessTest() {
    AtomicBoolean unlessCondition = new AtomicBoolean(true);
    AtomicBoolean hasRunCondition = new AtomicBoolean(false);

    Command command =
        new InstantCommand(() -> hasRunCondition.set(true)).unless(unlessCondition::get);

    command.schedule();
    CommandScheduler.getInstance().run();
    assertFalse(hasRunCondition.get());

    unlessCondition.set(false);
    command.schedule();
    CommandScheduler.getInstance().run();
    assertTrue(hasRunCondition.get());
  }

  @Test
  void onlyIfTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean onlyIfCondition = new AtomicBoolean(false);
      AtomicBoolean hasRunCondition = new AtomicBoolean(false);

      Command command =
          new InstantCommand(() -> hasRunCondition.set(true)).onlyIf(onlyIfCondition::get);

      scheduler.schedule(command);
      scheduler.run();
      assertFalse(hasRunCondition.get());

      onlyIfCondition.set(true);
      scheduler.schedule(command);
      scheduler.run();
      assertTrue(hasRunCondition.get());
    }
  }

  @Test
  void finallyDoTest() {
    AtomicInteger first = new AtomicInteger(0);
    AtomicInteger second = new AtomicInteger(0);

    Command command =
        new FunctionalCommand(
                () -> {},
                () -> {},
                interrupted -> {
                  if (!interrupted) {
                    first.incrementAndGet();
                  }
                },
                () -> true)
            .finallyDo(
                interrupted -> {
                  if (!interrupted) {
                    // to differentiate between "didn't run" and "ran before command's `end()`
                    second.addAndGet(1 + first.get());
                  }
                });

    command.schedule();
    assertEquals(0, first.get());
    assertEquals(0, second.get());
    CommandScheduler.getInstance().run();
    assertEquals(1, first.get());
    // if `second == 0`, neither of the lambdas ran.
    // if `second == 1`, the second lambda ran before the first one
    assertEquals(2, second.get());
  }

  // handleInterruptTest() implicitly tests the interrupt=true branch of finallyDo()
  @Test
  void handleInterruptTest() {
    AtomicInteger first = new AtomicInteger(0);
    AtomicInteger second = new AtomicInteger(0);

    Command command =
        new FunctionalCommand(
                () -> {},
                () -> {},
                interrupted -> {
                  if (interrupted) {
                    first.incrementAndGet();
                  }
                },
                () -> false)
            .handleInterrupt(
                () -> {
                  // to differentiate between "didn't run" and "ran before command's `end()`
                  second.addAndGet(1 + first.get());
                });

    command.schedule();
    CommandScheduler.getInstance().run();
    assertEquals(0, first.get());
    assertEquals(0, second.get());

    command.cancel();
    assertEquals(1, first.get());
    // if `second == 0`, neither of the lambdas ran.
    // if `second == 1`, the second lambda ran before the first one
    assertEquals(2, second.get());
  }

  @Test
  void withNameTest() {
    InstantCommand command = new InstantCommand();
    String name = "Named";
    Command named = command.withName(name);
    assertEquals(name, named.getName());
  }
}

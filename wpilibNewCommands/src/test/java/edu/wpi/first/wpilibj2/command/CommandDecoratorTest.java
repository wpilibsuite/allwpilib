// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertAll;
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
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command timeout = new RunCommand(() -> {}).withTimeout(0.1);

      scheduler.schedule(timeout);
      scheduler.run();

      assertTrue(scheduler.isScheduled(timeout));

      SimHooks.stepTiming(0.15);
      scheduler.run();

      assertFalse(scheduler.isScheduled(timeout));
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @Test
  void untilTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean finish = new AtomicBoolean();

      Command command = new RunCommand(() -> {}).until(finish::get);

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      finish.set(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void untilOrderTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean firstHasRun = new AtomicBoolean(false);
      AtomicBoolean firstWasPolled = new AtomicBoolean(false);

      Command first =
          new FunctionalCommand(
              () -> {},
              () -> firstHasRun.set(true),
              interrupted -> {},
              () -> {
                firstWasPolled.set(true);
                return true;
              });
      Command command =
          first.until(
              () -> {
                assertAll(
                    () -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
                return true;
              });

      scheduler.schedule(command);
      scheduler.run();

      assertAll(() -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
    }
  }

  @Test
  void andThenWaitUntilTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean finish = new AtomicBoolean();

      Command command = new RunCommand(() -> {}).andThenWaitUntil(finish::get);

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      finish.set(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  @ResourceLock("timing")
  void andThenWaitSecondsTest() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command awaitTimeout = new RunCommand(() -> {}).andThenWaitSeconds(0.1);

      scheduler.schedule(awaitTimeout);
      scheduler.run();

      assertTrue(scheduler.isScheduled(awaitTimeout));

      SimHooks.stepTiming(0.15);
      scheduler.run();

      assertFalse(scheduler.isScheduled(awaitTimeout));
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @Test
  void onlyWhileTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean run = new AtomicBoolean(true);

      Command command = new RunCommand(() -> {}).onlyWhile(run::get);

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      run.set(false);
      scheduler.run();

      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void onlyWhileOrderTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean firstHasRun = new AtomicBoolean(false);
      AtomicBoolean firstWasPolled = new AtomicBoolean(false);

      Command first =
          new FunctionalCommand(
              () -> {},
              () -> firstHasRun.set(true),
              interrupted -> {},
              () -> {
                firstWasPolled.set(true);
                return true;
              });
      Command command =
          first.onlyWhile(
              () -> {
                assertAll(
                    () -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
                return false;
              });

      scheduler.schedule(command);
      scheduler.run();

      assertAll(() -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
    }
  }

  @Test
  void ignoringDisableTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      var command = new RunCommand(() -> {}).ignoringDisable(true);

      setDSEnabled(false);

      scheduler.schedule(command);

      scheduler.run();
      assertTrue(scheduler.isScheduled(command));
    }
  }

  @Test
  void beforeStartingTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean finished = new AtomicBoolean();
      finished.set(false);

      Command command = new InstantCommand().beforeStarting(() -> finished.set(true));

      scheduler.schedule(command);

      assertTrue(finished.get());

      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      scheduler.run();

      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void andThenLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean finished = new AtomicBoolean(false);

      Command command = new InstantCommand().andThen(() -> finished.set(true));

      scheduler.schedule(command);

      assertFalse(finished.get());

      scheduler.run();

      assertTrue(finished.get());

      scheduler.run();

      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void andThenTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean(false);

      Command command1 = new InstantCommand();
      Command command2 = new InstantCommand(() -> condition.set(true));
      Command group = command1.andThen(command2);

      scheduler.schedule(group);

      assertFalse(condition.get());

      scheduler.run();

      assertTrue(condition.get());

      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void deadlineForTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean finish = new AtomicBoolean(false);

      Command dictator = new WaitUntilCommand(finish::get);
      Command endsBefore = new InstantCommand();
      Command endsAfter = new WaitUntilCommand(() -> false);

      Command group = dictator.deadlineFor(endsBefore, endsAfter);

      scheduler.schedule(group);
      scheduler.run();

      assertTrue(scheduler.isScheduled(group));

      finish.set(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void deadlineForOrderTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean dictatorHasRun = new AtomicBoolean(false);
      AtomicBoolean dictatorWasPolled = new AtomicBoolean(false);

      Command dictator =
          new FunctionalCommand(
              () -> {},
              () -> dictatorHasRun.set(true),
              interrupted -> {},
              () -> {
                dictatorWasPolled.set(true);
                return true;
              });
      Command other =
          new RunCommand(
              () ->
                  assertAll(
                      () -> assertTrue(dictatorHasRun.get()),
                      () -> assertTrue(dictatorWasPolled.get())));

      Command group = dictator.deadlineFor(other);

      scheduler.schedule(group);
      scheduler.run();

      assertAll(() -> assertTrue(dictatorHasRun.get()), () -> assertTrue(dictatorWasPolled.get()));
    }
  }

  @Test
  void alongWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean finish = new AtomicBoolean(false);

      Command command1 = new WaitUntilCommand(finish::get);
      Command command2 = new InstantCommand();

      Command group = command1.alongWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertTrue(scheduler.isScheduled(group));

      finish.set(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void alongWithOrderTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean firstHasRun = new AtomicBoolean(false);
      AtomicBoolean firstWasPolled = new AtomicBoolean(false);

      Command command1 =
          new FunctionalCommand(
              () -> {},
              () -> firstHasRun.set(true),
              interrupted -> {},
              () -> {
                firstWasPolled.set(true);
                return true;
              });
      Command command2 =
          new RunCommand(
              () ->
                  assertAll(
                      () -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get())));

      Command group = command1.alongWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertAll(() -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
    }
  }

  @Test
  void raceWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command1 = new WaitUntilCommand(() -> false);
      Command command2 = new InstantCommand();

      Command group = command1.raceWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void raceWithOrderTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean firstHasRun = new AtomicBoolean(false);
      AtomicBoolean firstWasPolled = new AtomicBoolean(false);

      Command command1 =
          new FunctionalCommand(
              () -> {},
              () -> firstHasRun.set(true),
              interrupted -> {},
              () -> {
                firstWasPolled.set(true);
                return true;
              });
      Command command2 =
          new RunCommand(
              () -> {
                assertTrue(firstHasRun.get());
                assertTrue(firstWasPolled.get());
              });

      Command group = command1.raceWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertAll(() -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
    }
  }

  @Test
  void unlessTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean hasRun = new AtomicBoolean(false);
      AtomicBoolean unlessCondition = new AtomicBoolean(true);

      Command command = new InstantCommand(() -> hasRun.set(true)).unless(unlessCondition::get);

      scheduler.schedule(command);
      scheduler.run();
      assertFalse(hasRun.get());

      unlessCondition.set(false);
      scheduler.schedule(command);
      scheduler.run();
      assertTrue(hasRun.get());
    }
  }

  @Test
  void onlyIfTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean hasRun = new AtomicBoolean(false);
      AtomicBoolean onlyIfCondition = new AtomicBoolean(false);

      Command command = new InstantCommand(() -> hasRun.set(true)).onlyIf(onlyIfCondition::get);

      scheduler.schedule(command);
      scheduler.run();
      assertFalse(hasRun.get());

      onlyIfCondition.set(true);
      scheduler.schedule(command);
      scheduler.run();
      assertTrue(hasRun.get());
    }
  }

  @Test
  void finallyDoTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
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

      scheduler.schedule(command);
      assertEquals(0, first.get());
      assertEquals(0, second.get());
      scheduler.run();
      assertEquals(1, first.get());
      // if `second == 0`, neither of the lambdas ran.
      // if `second == 1`, the second lambda ran before the first one
      assertEquals(2, second.get());
    }
  }

  // handleInterruptTest() implicitly tests the interrupt=true branch of finallyDo()
  @Test
  void handleInterruptTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
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

      scheduler.schedule(command);
      scheduler.run();
      assertEquals(0, first.get());
      assertEquals(0, second.get());

      scheduler.cancel(command);
      assertEquals(1, first.get());
      // if `second == 0`, neither of the lambdas ran.
      // if `second == 1`, the second lambda ran before the first one
      assertEquals(2, second.get());
    }
  }

  @Test
  void withNameTest() {
    InstantCommand command = new InstantCommand();
    String name = "Named";
    Command named = command.withName(name);
    assertEquals(name, named.getName());
  }
}

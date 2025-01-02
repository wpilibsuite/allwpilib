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
    try {
      Command timeout = Commands.idle().withTimeout(0.1);

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
    AtomicBoolean finish = new AtomicBoolean();

    Command command = Commands.idle().until(finish::get);

    scheduler.schedule(command);
    scheduler.run();

    assertTrue(scheduler.isScheduled(command));

    finish.set(true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(command));
  }

  @Test
  void untilOrderTest() {
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

  @Test
  void onlyWhileTest() {
    AtomicBoolean run = new AtomicBoolean(true);

    Command command = Commands.idle().onlyWhile(run::get);

    scheduler.schedule(command);
    scheduler.run();

    assertTrue(scheduler.isScheduled(command));

    run.set(false);
    scheduler.run();

    assertFalse(scheduler.isScheduled(command));
  }

  @Test
  void onlyWhileOrderTest() {
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

  @Test
  void ignoringDisableTest() {
    var command = Commands.idle().ignoringDisable(true);

    setDSEnabled(false);

    scheduler.schedule(command);

    scheduler.run();
    assertTrue(scheduler.isScheduled(command));
  }

  @Test
  void beforeStartingTest() {
    AtomicBoolean finished = new AtomicBoolean();
    finished.set(false);

    Command command = Commands.none().beforeStarting(() -> finished.set(true));

    scheduler.schedule(command);

    assertTrue(finished.get());

    scheduler.run();

    assertTrue(scheduler.isScheduled(command));

    scheduler.run();

    assertFalse(scheduler.isScheduled(command));
  }

  @Test
  void andThenLambdaTest() {
    AtomicBoolean finished = new AtomicBoolean(false);

    Command command = Commands.none().andThen(() -> finished.set(true));

    scheduler.schedule(command);

    assertFalse(finished.get());

    scheduler.run();

    assertTrue(finished.get());

    scheduler.run();

    assertFalse(scheduler.isScheduled(command));
  }

  @Test
  void andThenTest() {
    AtomicBoolean condition = new AtomicBoolean(false);

    Command command1 = Commands.none();
    Command command2 = Commands.runOnce(() -> condition.set(true));
    Command group = command1.andThen(command2);

    scheduler.schedule(group);

    assertFalse(condition.get());

    scheduler.run();

    assertTrue(condition.get());

    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void deadlineForTest() {
    AtomicBoolean finish = new AtomicBoolean(false);

    Command dictator = Commands.waitUntil(finish::get);
    Command endsBefore = Commands.none();
    Command endsAfter = Commands.idle();

    Command group = dictator.deadlineFor(endsBefore, endsAfter);

    scheduler.schedule(group);
    scheduler.run();

    assertTrue(scheduler.isScheduled(group));

    finish.set(true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void deadlineForOrderTest() {
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
        Commands.run(
            () ->
                assertAll(
                    () -> assertTrue(dictatorHasRun.get()),
                    () -> assertTrue(dictatorWasPolled.get())));

    Command group = dictator.deadlineFor(other);

    scheduler.schedule(group);
    scheduler.run();

    assertAll(() -> assertTrue(dictatorHasRun.get()), () -> assertTrue(dictatorWasPolled.get()));
  }

  @Test
  void withDeadlineTest() {
    AtomicBoolean finish = new AtomicBoolean(false);

    Command endsBeforeGroup = Commands.none().withDeadline(Commands.waitUntil(finish::get));
    scheduler.schedule(endsBeforeGroup);
    scheduler.run();
    assertTrue(scheduler.isScheduled(endsBeforeGroup));
    finish.set(true);
    scheduler.run();
    assertFalse(scheduler.isScheduled(endsBeforeGroup));
    finish.set(false);

    Command endsAfterGroup = Commands.idle().withDeadline(Commands.waitUntil(finish::get));
    scheduler.schedule(endsAfterGroup);
    scheduler.run();
    assertTrue(scheduler.isScheduled(endsAfterGroup));
    finish.set(true);
    scheduler.run();
    assertFalse(scheduler.isScheduled(endsAfterGroup));
  }

  @Test
  void withDeadlineOrderTest() {
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
        Commands.run(
            () ->
                assertAll(
                    () -> assertTrue(dictatorHasRun.get()),
                    () -> assertTrue(dictatorWasPolled.get())));
    Command group = other.withDeadline(dictator);
    scheduler.schedule(group);
    scheduler.run();
    assertAll(() -> assertTrue(dictatorHasRun.get()), () -> assertTrue(dictatorWasPolled.get()));
  }

  @Test
  void alongWithTest() {
    AtomicBoolean finish = new AtomicBoolean(false);

    Command command1 = Commands.waitUntil(finish::get);
    Command command2 = Commands.none();

    Command group = command1.alongWith(command2);

    scheduler.schedule(group);
    scheduler.run();

    assertTrue(scheduler.isScheduled(group));

    finish.set(true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void alongWithOrderTest() {
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

  @Test
  void raceWithTest() {
    Command command1 = Commands.idle();
    Command command2 = Commands.none();

    Command group = command1.raceWith(command2);

    scheduler.schedule(group);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void raceWithOrderTest() {
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
        Commands.run(
            () -> {
              assertTrue(firstHasRun.get());
              assertTrue(firstWasPolled.get());
            });

    Command group = command1.raceWith(command2);

    scheduler.schedule(group);
    scheduler.run();

    assertAll(() -> assertTrue(firstHasRun.get()), () -> assertTrue(firstWasPolled.get()));
  }

  @Test
  void unlessTest() {
    AtomicBoolean hasRun = new AtomicBoolean(false);
    AtomicBoolean unlessCondition = new AtomicBoolean(true);

    Command command = Commands.runOnce(() -> hasRun.set(true)).unless(unlessCondition::get);

    scheduler.schedule(command);
    scheduler.run();
    assertFalse(hasRun.get());

    unlessCondition.set(false);
    scheduler.schedule(command);
    scheduler.run();
    assertTrue(hasRun.get());
  }

  @Test
  void onlyIfTest() {
    AtomicBoolean hasRun = new AtomicBoolean(false);
    AtomicBoolean onlyIfCondition = new AtomicBoolean(false);

    Command command = Commands.runOnce(() -> hasRun.set(true)).onlyIf(onlyIfCondition::get);

    scheduler.schedule(command);
    scheduler.run();
    assertFalse(hasRun.get());

    onlyIfCondition.set(true);
    scheduler.schedule(command);
    scheduler.run();
    assertTrue(hasRun.get());
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

    scheduler.schedule(command);
    assertEquals(0, first.get());
    assertEquals(0, second.get());
    scheduler.run();
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

  @Test
  void withNameTest() {
    Command command = Commands.none();
    String name = "Named";
    Command named = command.withName(name);
    assertEquals(name, named.getName());
  }
}

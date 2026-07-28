// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Seconds;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.junit.jupiter.api.Test;
import org.wpilib.system.RobotController;

class CoroutineTest extends CommandTestBase {
  @Test
  void waitUntilConditionMet() {
    AtomicBoolean condition = new AtomicBoolean(false);
    AtomicReference<Coroutine.WaitResult> result = new AtomicReference<>();

    var command =
        Command.noRequirements(co -> result.set(co.waitUntil(condition::get, Seconds.of(1.0))))
            .named("Wait Until Condition Met");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // Condition not met yet, should still be running
    assertTrue(m_scheduler.isRunning(command));

    condition.set(true);
    m_scheduler.run();

    // Condition met, should have finished
    assertEquals(Coroutine.WaitResult.CONDITION_MET, result.get());
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void waitUntilTimeout() {
    AtomicBoolean condition = new AtomicBoolean(false);
    AtomicReference<Coroutine.WaitResult> result = new AtomicReference<>();
    AtomicReference<Long> currentTime = new AtomicReference<>(0L);

    RobotController.setTimeSource(currentTime::get);

    var command =
        Command.noRequirements(co -> result.set(co.waitUntil(condition::get, Seconds.of(1.0))))
            .named("Wait Until Timeout");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // Still running, time is 0
    assertTrue(m_scheduler.isRunning(command));

    // Advance time to 0.5s
    currentTime.set(500_000L);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    // Advance time to 1.1s (past 1.0s timeout)
    currentTime.set(1_100_000L);
    m_scheduler.run();

    // Should have timed out
    assertEquals(Coroutine.WaitResult.TIMED_OUT, result.get());
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void waitUntilImmediateConditionMet() {
    AtomicReference<Coroutine.WaitResult> result = new AtomicReference<>();

    var command =
        Command.noRequirements(co -> result.set(co.waitUntil(() -> true, Seconds.of(1.0))))
            .named("Wait Until Immediate Condition Met");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // Condition met immediately, should have finished in one run
    assertEquals(Coroutine.WaitResult.CONDITION_MET, result.get());
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void waitUntilConditionMetExactlyAtTimeout() {
    AtomicBoolean condition = new AtomicBoolean(false);
    AtomicReference<Coroutine.WaitResult> result = new AtomicReference<>();
    AtomicReference<Long> currentTime = new AtomicReference<>(0L);

    RobotController.setTimeSource(currentTime::get);

    var command =
        Command.noRequirements(co -> result.set(co.waitUntil(condition::get, Seconds.of(1.0))))
            .named("Wait Until Condition Met Exactly At Timeout");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // Advance time to exactly 1.0s and set condition
    currentTime.set(1_000_000L);
    condition.set(true);
    m_scheduler.run();

    // Condition met, even though time is exactly at timeout.
    // The implementation checks the condition BEFORE the timeout in the while loop.
    assertEquals(Coroutine.WaitResult.CONDITION_MET, result.get());
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void waitUntilNullParamThrows() {
    var command =
        Command.noRequirements(
                co -> {
                  assertThrows(
                      NullPointerException.class, () -> co.waitUntil(null, Seconds.of(1.0)));
                  assertThrows(NullPointerException.class, () -> co.waitUntil(() -> true, null));
                })
            .named("Wait Until Null Param Throws");

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void forkMany() {
    var a = new NullCommand();
    var b = new NullCommand();
    var c = new NullCommand();

    var all =
        Command.noRequirements(
                co -> {
                  co.fork(a, b, c);
                  co.park();
                })
            .named("Fork Many");

    m_scheduler.schedule(all);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(a));
    assertTrue(m_scheduler.isRunning(b));
    assertTrue(m_scheduler.isRunning(c));
  }

  @Test
  void yieldInSynchronizedBlock() {
    Object mutex = new Object();
    AtomicInteger i = new AtomicInteger(0);

    var yieldInSynchronized =
        Command.noRequirements(
                co -> {
                  while (true) {
                    synchronized (mutex) {
                      i.incrementAndGet();
                      co.yield();
                    }
                  }
                })
            .named("Yield In Synchronized Block");

    m_scheduler.schedule(yieldInSynchronized);
    m_scheduler.run();
    assertEquals(1, i.get());
  }

  @Test
  void yieldInLockBody() {
    Lock lock = new ReentrantLock();
    AtomicInteger i = new AtomicInteger(0);

    var yieldInLock =
        Command.noRequirements(
                co -> {
                  while (true) {
                    lock.lock();
                    try {
                      i.incrementAndGet();
                      co.yield();
                    } finally {
                      lock.unlock();
                    }
                  }
                })
            .named("Increment In Lock Block");

    m_scheduler.schedule(yieldInLock);
    m_scheduler.run();
    assertEquals(1, i.get());
  }

  @Test
  void coroutineEscapingCommand() {
    AtomicReference<Runnable> escapeeCallback = new AtomicReference<>();

    var badCommand =
        Command.noRequirements(
                co -> {
                  escapeeCallback.set(co::yield);
                })
            .named("Bad Command");

    m_scheduler.schedule(badCommand);
    m_scheduler.run();

    var error = assertThrows(IllegalStateException.class, escapeeCallback.get()::run);
    assertEquals("Coroutines can only be used by the command bound to them", error.getMessage());
  }

  @Test
  @SuppressWarnings("CoroutineMayNotBeInScope")
  void usingParentCoroutineInChildThrows() {
    var parent =
        Command.noRequirements(
                parentCoroutine -> {
                  parentCoroutine.await(
                      Command.noRequirements(
                              childCoroutine -> {
                                parentCoroutine.yield();
                              })
                          .named("Child"));
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    var error = assertThrows(IllegalStateException.class, m_scheduler::run);
    assertEquals("Coroutines can only be used by the command bound to them", error.getMessage());
  }

  @Test
  void awaitAnyCleansUp() {
    AtomicBoolean firstRan = new AtomicBoolean(false);
    AtomicBoolean secondRan = new AtomicBoolean(false);
    AtomicBoolean ranAfterAwait = new AtomicBoolean(false);

    var firstInner = Command.noRequirements(c2 -> firstRan.set(true)).named("First");
    var secondInner =
        Command.noRequirements(
                c2 -> {
                  secondRan.set(true);
                  c2.park();
                })
            .named("Second");

    var outer =
        Command.noRequirements(
                co -> {
                  co.awaitAny(firstInner, secondInner);

                  ranAfterAwait.set(true);
                  co.park(); // prevent exiting
                })
            .named("Command");

    m_scheduler.schedule(outer);
    m_scheduler.run();

    // Everything should have run...
    assertTrue(firstRan.get());
    assertTrue(secondRan.get());
    assertTrue(ranAfterAwait.get());

    // But only the outer command should still be running; secondInner should have been canceled
    assertEquals(List.of(outer), m_scheduler.getRunningCommands());
  }
}

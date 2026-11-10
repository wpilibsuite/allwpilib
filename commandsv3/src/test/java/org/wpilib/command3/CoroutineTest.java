// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Nanoseconds;
import static org.wpilib.units.Units.Seconds;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.junit.jupiter.api.Test;
import org.wpilib.system.RobotController;

@SuppressWarnings("PMD.CompareObjectsWithEquals")
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
    AtomicReference<Coroutine.WaitResult> result = new AtomicReference<>();
    AtomicReference<Long> currentTime = new AtomicReference<>(0L);

    RobotController.setTimeSource(currentTime::get);

    var command =
        Command.noRequirements(co -> result.set(co.waitUntil(() -> false, Seconds.of(1.0))))
            .named("Wait Until Timeout");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // Still running, time is 0
    assertTrue(m_scheduler.isRunning(command));

    // Advance time to 0.5s
    currentTime.set((long) Seconds.of(0.5).in(Nanoseconds));
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    // Advance time to 1.1s (past 1.0s timeout)
    currentTime.set((long) Seconds.of(1.1).in(Nanoseconds));
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
    currentTime.set(1_000_000_000L);
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
  void forkResultAwaitCompletion() {
    var signal = new AtomicBoolean(false);
    var waitingCommand =
        Command.noRequirements(coroutine -> coroutine.waitUntil(signal::get))
            .named("Wait For Signal");

    var parent =
        Command.noRequirements(
                coroutine -> {
                  var forkResult = coroutine.fork(waitingCommand);
                  forkResult.awaitCompletion();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent, waitingCommand), m_scheduler.getRunningCommands());

    // Run the scheduler a few times to ensure `awaitCompletion()` continues to wait
    m_scheduler.run();
    m_scheduler.run();
    m_scheduler.run();
    m_scheduler.run();
    assertEquals(List.of(parent, waitingCommand), m_scheduler.getRunningCommands());

    signal.set(true);
    m_scheduler.run();
    assertEquals(List.of(), m_scheduler.getRunningCommands());
  }

  @Test
  void forkResultAwaitCompletionOneShot() {
    var ran = new AtomicBoolean(false);
    var oneShot = Command.noRequirements(_ -> {}).named("OneShot");
    var parent =
        Command.noRequirements(
                coroutine -> {
                  var forkResult = coroutine.fork(oneShot);
                  assertTrue(forkResult.successful());
                  forkResult.awaitCompletion();
                  ran.set(true);
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertTrue(ran.get());
    assertEquals(List.of(), m_scheduler.getRunningCommands());
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
  @SuppressWarnings("WPILib.CoroutineMayNotBeInScope")
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

  @Test
  void awaitAnyDoesNotCancelRescheduledCommand() {
    AtomicBoolean ranAfterAwait = new AtomicBoolean(false);

    var c1 = Command.noRequirements(Coroutine::park).named("C1");
    var c2 = Command.noRequirements(Coroutine::park).named("C2");

    var parent =
        Command.noRequirements(
                co -> {
                  co.awaitAny(c1, c2);
                  ranAfterAwait.set(true);
                  co.park();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run(); // first run: parent forks c1 and c2, then enters a waiting state

    // immediately cancel and reschedule c1 before the parent resumes
    m_scheduler.cancel(c1);
    m_scheduler.schedule(c1);

    // Second run: parent resumes, sees c1's original run completed, exits awaitAny and cancels c2.
    // It does not cancel c1's rescheduled run because of the mismatched run ID.
    // parent then sets ranAfterAwait to true and parks
    m_scheduler.run();

    assertTrue(ranAfterAwait.get());
    assertEquals(List.of(parent, c1), m_scheduler.getRunningCommands());

    assertSchedulerEvent(
        SchedulerEvent.Canceled.class,
        c -> c.command() == c2,
        "C2 should have been canceled by awaitAny");
  }

  @Test
  void forkResultDelayedAwaitCompletionAfterCompletion() {
    AtomicBoolean childRan = new AtomicBoolean(false);
    AtomicBoolean parentDone = new AtomicBoolean(false);

    var child =
        Command.noRequirements(
                co -> {
                  childRan.set(true);
                })
            .named("Child");

    var parent =
        Command.noRequirements(
                co -> {
                  var forkResult = co.fork(child);
                  // Yield while child runs to completion
                  co.yield();
                  co.yield();
                  // Child has completed before awaitCompletion is called
                  forkResult.awaitCompletion();
                  parentDone.set(true);
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    m_scheduler.run();
    assertTrue(childRan.get());
    assertFalse(m_scheduler.isRunning(child));

    m_scheduler.run();
    assertTrue(parentDone.get());
    assertFalse(m_scheduler.isRunning(parent));
  }

  @Test
  void forkResultDelayedAwaitCompletionWithRescheduledCommand() {
    AtomicInteger childRunCount = new AtomicInteger(0);
    AtomicBoolean parentDone = new AtomicBoolean(false);

    var child =
        Command.noRequirements(
                co -> {
                  if (childRunCount.incrementAndGet() > 1) {
                    co.park();
                  }
                })
            .named("Child");

    var parent =
        Command.noRequirements(
                co -> {
                  var forkResult = co.fork(child);
                  // Yield while child finishes its first run
                  co.yield();
                  co.yield();
                  // awaitCompletion should only wait for the original run, not the new run
                  forkResult.awaitCompletion();
                  parentDone.set(true);
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(1, childRunCount.get());
    assertFalse(m_scheduler.isRunning(child));

    // Reschedule child externally with a new run ID
    m_scheduler.schedule(child);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(child));
    assertEquals(2, childRunCount.get());
    assertFalse(parentDone.get());

    // Next scheduler run: parent resumes, calls awaitCompletion()
    // It should immediately return and finish without waiting for child's second run
    m_scheduler.run();
    assertTrue(parentDone.get());
    assertFalse(m_scheduler.isRunning(parent));
    assertTrue(m_scheduler.isRunning(child));
  }

  @Test
  void forkResultDelayedAwaitCompletionPartialReschedule() {
    AtomicInteger c1RunCount = new AtomicInteger(0);
    AtomicBoolean c2Done = new AtomicBoolean(false);
    AtomicBoolean parentDone = new AtomicBoolean(false);

    var c1 =
        Command.noRequirements(
                co -> {
                  if (c1RunCount.incrementAndGet() > 1) {
                    co.park();
                  }
                })
            .named("C1");

    var c2 =
        Command.noRequirements(
                co -> {
                  co.waitUntil(c2Done::get);
                })
            .named("C2");

    var parent =
        Command.noRequirements(
                co -> {
                  var forkResult = co.fork(c1, c2);
                  // Yield while c1 completes
                  co.yield();
                  // Await original fork result
                  forkResult.awaitCompletion();
                  parentDone.set(true);
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertEquals(1, c1RunCount.get());
    assertFalse(m_scheduler.isRunning(c1));
    assertTrue(m_scheduler.isRunning(c2));

    // Reschedule c1 externally (which will park and stay running on its 2nd run)
    m_scheduler.schedule(c1);
    m_scheduler.run();
    assertEquals(2, c1RunCount.get());
    assertTrue(m_scheduler.isRunning(c1));
    assertTrue(m_scheduler.isRunning(c2));
    assertFalse(parentDone.get());

    // Complete c2
    c2Done.set(true);
    m_scheduler.run();
    assertTrue(parentDone.get());
    assertFalse(m_scheduler.isRunning(parent));
    assertTrue(m_scheduler.isRunning(c1));
  }

  @Test
  void forkResultDelayedAwaitCompletionLargeSet() {
    int n = 80;
    List<Command> children = new ArrayList<>();
    List<AtomicBoolean> completeFlags = new ArrayList<>();
    AtomicIntegerArray runCounts = new AtomicIntegerArray(n);
    AtomicBoolean parentDone = new AtomicBoolean(false);

    for (int i = 0; i < n; i++) {
      final int idx = i;
      var flag = new AtomicBoolean(false);
      completeFlags.add(flag);
      children.add(
          Command.noRequirements(
                  co -> {
                    int count = runCounts.incrementAndGet(idx);
                    if (count == 1) {
                      co.waitUntil(flag::get);
                    } else {
                      co.park();
                    }
                  })
              .named("LargeChild[" + idx + "]"));
    }

    var parent =
        Command.noRequirements(
                co -> {
                  var forkResult = co.fork(children);
                  // Yield while initial batch completes and reschedules
                  co.yield();
                  // Await original fork completion
                  forkResult.awaitCompletion();
                  parentDone.set(true);
                })
            .named("ParentLarge");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    // Complete subset across word 0 (0..39) and word 1 (64..71)
    for (int i = 0; i < 40; i++) {
      completeFlags.get(i).set(true);
    }
    for (int i = 64; i < 72; i++) {
      completeFlags.get(i).set(true);
    }
    m_scheduler.run();

    // Reschedule a subset: 20..39 (word 0) and 68..71 (word 1)
    for (int i = 20; i < 40; i++) {
      m_scheduler.schedule(children.get(i));
    }
    for (int i = 68; i < 72; i++) {
      m_scheduler.schedule(children.get(i));
    }
    m_scheduler.run();

    assertFalse(parentDone.get());
    assertTrue(m_scheduler.isRunning(parent));

    // Complete the remaining original children: 40..63 and 72..79
    for (int i = 40; i < 64; i++) {
      completeFlags.get(i).set(true);
    }
    for (int i = 72; i < n; i++) {
      completeFlags.get(i).set(true);
    }
    m_scheduler.run();

    // Parent should finish immediately without blocking on the rescheduled children
    assertTrue(parentDone.get());
    assertFalse(m_scheduler.isRunning(parent));

    // Rescheduled children should still be running
    for (int i = 20; i < 40; i++) {
      assertTrue(m_scheduler.isRunning(children.get(i)));
    }
    for (int i = 68; i < 72; i++) {
      assertTrue(m_scheduler.isRunning(children.get(i)));
    }
  }
}

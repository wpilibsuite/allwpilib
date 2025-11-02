// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.junit.jupiter.api.Test;

class CoroutineTest extends CommandTestBase {
  @Test
  void forkMany() {
    var a = new NullCommand();
    var b = new NullCommand();
    var c = new NullCommand();

    var all =
        Command.noRequirements()
            .executing(
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
        Command.noRequirements()
            .executing(
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

    var error = assertThrows(IllegalStateException.class, m_scheduler::run);
    assertEquals(
        "Coroutine.yield() cannot be called inside a synchronized block or method. "
            + "Consider using a Lock instead of synchronized, "
            + "or rewrite your code to avoid locks and mutexes altogether.",
        error.getMessage());
  }

  @Test
  void yieldInLockBody() {
    Lock lock = new ReentrantLock();
    AtomicInteger i = new AtomicInteger(0);

    var yieldInLock =
        Command.noRequirements()
            .executing(
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
        Command.noRequirements()
            .executing(
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
        Command.noRequirements()
            .executing(
                parentCoroutine -> {
                  parentCoroutine.await(
                      Command.noRequirements()
                          .executing(
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

    var firstInner = Command.noRequirements().executing(c2 -> firstRan.set(true)).named("First");
    var secondInner =
        Command.noRequirements()
            .executing(
                c2 -> {
                  secondRan.set(true);
                  c2.park();
                })
            .named("Second");

    var outer =
        Command.noRequirements()
            .executing(
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
    assertEquals(Set.of(outer), m_scheduler.getRunningCommands());
  }
}

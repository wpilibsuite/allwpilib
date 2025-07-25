// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.wpilibj.RobotController;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class CoroutineTest {
  Scheduler m_scheduler;

  @BeforeEach
  void setup() {
    m_scheduler = new Scheduler();
    RobotController.setTimeSource(() -> System.nanoTime() / 1000L);
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

    try {
      m_scheduler.run();
      fail("Monitor pinned exception should have been thrown");
    } catch (IllegalStateException expected) {
      assertEquals(
          "Coroutine.yield() cannot be called inside a synchronized block or method. "
              + "Consider using a Lock instead of synchronized, "
              + "or rewrite your code to avoid locks and mutexes altogether.",
          expected.getMessage());
    }
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

    try {
      escapeeCallback.get().run();
      fail("Calling coroutine.yield() outside of a command should error");
    } catch (IllegalStateException expected) {
      assertEquals("Coroutines can only be used while running in a command", expected.getMessage());
    }
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

    // But only the outer command should still be running; secondInner should have been cancelled
    assertEquals(Set.of(outer), m_scheduler.getRunningCommands());
  }
}
